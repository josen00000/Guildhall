#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/CubeSphere.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/DebugRender.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern DevConsole*		g_theConsole;
extern Game*			g_theGame;

Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_gameState = GAME_STATE_LOADING;
	m_developMode	= false;
	m_noClip		= false;
	m_isPause		= false;
	m_debugCamera	= false;
	m_isAppQuit		= false;
	m_isAttractMode	= false;

	// debug render system
	DebugRenderSystemStartup( g_theRenderer, m_gameCamera );
	CreateShaderNames();
	CreateTestObjects();
	//CreateDebugRenderObjects();

	EventCallbackFunctionPtr SetAmbientColorFuncPtr			= LightCommandSetAmbientColor;
	EventCallbackFunctionPtr SetLightColorFuncPtr			= LightCommandSetLightColor;

	g_theConsole->AddCommandToCommandList( std::string( "set_ambient_color"), std::string(" set ambient color( color = vec3)" ), SetAmbientColorFuncPtr );
	g_theConsole->AddCommandToCommandList( std::string( "set_light_color"), std::string(" set light color( color = vec3)" ), SetLightColorFuncPtr );
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_sphereMesh;
	delete m_tesMesh;
	SELF_SAFE_RELEASE(m_lightQuadMesh);
	SELF_SAFE_RELEASE(m_cubeSphereMesh);
	SELF_SAFE_RELEASE(m_lightCubeMesh);
	SELF_SAFE_RELEASE(m_lightSphereMesh);

	// clear game objects
	SELF_SAFE_RELEASE(m_lightQuadObject);
	SELF_SAFE_RELEASE(m_lightCubeObject);
	SELF_SAFE_RELEASE(m_lightSphereObject);

	for( int i = 0; i < m_sphereObjects.size(); i++ ){
		delete m_sphereObjects[i];
	}

	DebugRenderSystemShutdown();
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
}

void Game::Reset()
{
	Startup();
}

void Game::Update( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() ) {
		HandleDevKeyboardInput( deltaSeconds );
		HandleDebugKeyboardInput( deltaSeconds );
		HandleLightKeyboardInput( deltaSeconds );
	}
	UpdateMeshes( deltaSeconds );
	UpdateLighting( deltaSeconds );
	//UpdateLightObjects( deltaSeconds );
	UpdateUI( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	CreateDebugScreen();
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateLighting( float deltaSeconds )
{
	static float animationSeconds = 0.f;
	float theta = animationSeconds * 30.f;
	float dist = 8.f;
	if( m_isLightFollowCamera ) {
		m_lightPos = m_gameCamera->GetPosition();
	}
	else {
		DebugAddWorldPoint( m_lightPos, 1.f, m_lightColor, 0.1f, DEBUG_RENDER_ALWAYS );
	}
	if( m_isLightFollowAnimation ) {
		animationSeconds += deltaSeconds;
		m_lightPos = Vec3( CosDegrees( theta ) * dist, 0, -SinDegrees( theta ) * dist ) + Vec3( 0.f, 0.f, -5.f );
	}
	else {
		animationSeconds = 0.f;
	}

	g_theRenderer->EnablePointLight( 0, m_lightPos, m_lightColor, m_lightIntensity, m_diffuseAttenuation );
}

void Game::CreateShaderNames()
{
	m_shaderNames.push_back( std::string( "data/Shader/default.hlsl" ) );
	m_shaderNames.push_back( std::string( "data/Shader/lit.hlsl" ) );
	m_shaderNames.push_back( std::string( "data/Shader/normal.hlsl" ) );
	m_shaderNames.push_back( std::string( "data/Shader/tangent.hlsl" ) );
	m_shaderNames.push_back( std::string( "data/Shader/bitangent.hlsl" ) );
	m_shaderNames.push_back( std::string( "data/Shader/surface_normal.hlsl" ) );
}

void Game::CreateDebugScreen()
{
	float deltaHeight = 3.f;
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f ), Vec2::ZERO, 3.f, Rgba8::WHITE, "Ambient light intensity: %.2f. [9,0]", m_ambientLightIntensity );
	std::string attenuationText = "Attenuation: " + m_diffuseAttenuation.GetText();
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Diffuse Attenuation: %.2f, %.2f, %.2f ", m_diffuseAttenuation.x, m_diffuseAttenuation.y, m_diffuseAttenuation.z );
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 2 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Attenuation: %.2f, %.2f, %.2f ", m_specularAttenuation.x, m_specularAttenuation.y, m_specularAttenuation.z );
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 3 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Light intensity: %.2f. [-,+] ", m_lightIntensity );
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 4 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Factor: %.2f. ['[',']'] ", m_specularFactor );
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 5 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Pow: %.2f. [';' , '''] ", m_specularPow );
	std::string shaderName = "Shader : " + m_shaderNames[m_currentShaderIndex];
	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 6 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, shaderName );

}

void Game::BindCurrentShader() const
{
	std::string currentShaderName = m_shaderNames[m_currentShaderIndex];
	g_theRenderer->BindShader( currentShaderName );
}

void Game::UpdateMeshes( float deltaSeconds )
{
	UpdateSphereMeshes( deltaSeconds );
}

void Game::UpdateSphereMeshes( float deltaSeconds )
{
	static float totalSeconds = 0.f;
	totalSeconds += deltaSeconds;
	Vec2 XY = Vec2( 10, 0 );
	float deltaDegrees = 360.f / 16;
	float degrees = 0.f;
	for( int i = 0; i < m_sphereObjects.size(); i++ ){
		float offset = 30.f * i;
		float rotCoe = SinDegrees( offset + totalSeconds );
		Vec3 rotation = Vec3( 0, 0, rotCoe * 180 );
		m_sphereObjects[i]->SetRotation( rotation );
		// pos
		XY.SetAngleDegrees( degrees + totalSeconds * 20 );
		degrees += deltaDegrees;
		m_sphereObjects[i]->SetPosition( Vec3( XY.x, XY.y, -20 ) );
	}
}

void Game::HandleDevKeyboardInput( float deltaSeconds )
{
	CheckIfExit();
	HandleCameraMovement( deltaSeconds );
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_O ) ){
		g_theInputSystem->SetCursorMode( CURSOR_RELATIVE );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P) ){
		g_theInputSystem->SetCursorMode( CURSOR_ABSOLUTE );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_C) ){
		g_theInputSystem->ClipSystemCursor();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_V ) ){
		g_theInputSystem->UnClipSystemCursor();
	}
}

void Game::HandleDebugKeyboardInput( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	Transform trans = m_gameCamera->m_transform;
	Mat44 transMat = trans.ToMatrix();
	float debugTime = -1.f;
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1 ) ){
		DebugAddWorldPoint( trans.m_pos, 1.f, Rgba8::RED, Rgba8::BLUE, debugTime, DEBUG_RENDER_USE_DEPTH );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
		DebugAddWorldLine( trans.m_pos, Rgba8::RED, Rgba8::RED,  trans.m_pos - transMat.GetKBasis3D() * 3.f, Rgba8::BLUE, Rgba8::BLUE, debugTime, DEBUG_RENDER_USE_DEPTH );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_3 ) ) {
		DebugAddWorldArrow( trans.m_pos, Rgba8::RED, Rgba8::RED,  trans.m_pos - transMat.GetKBasis3D() * 3.f, Rgba8::BLUE, Rgba8::BLUE, debugTime, DEBUG_RENDER_USE_DEPTH );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_4 ) ) {
		DebugAddWorldQuad( Vec3( 0.f, 0.f, -5.f), Vec3( 3.f, 0.f, -5.f ), Vec3( 3.f, 5.f, -5.f ), Vec3( 0.f, 5.f, -5.f ), Rgba8::BLUE, Rgba8::BLACK, debugTime, DEBUG_RENDER_ALWAYS );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_5 ) ) {
		AABB3 box = AABB3( trans.m_pos - Vec3( 10.f, 10.f, 10.f ),trans.m_pos + Vec3( 10.f ) );
		DebugAddWorldWireBounds( box, Vec3( 0.f, 1.f, 0.f), Rgba8::RED, Rgba8::BLUE, debugTime, DEBUG_RENDER_ALWAYS );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_6 ) ) {
		DebugAddWorldWireSphere( trans.m_pos, 10.f, Rgba8::RED, Rgba8::BLUE, debugTime, DEBUG_RENDER_ALWAYS );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_7 ) ) {
		DebugAddWorldBasis( transMat, Rgba8::WHITE, Rgba8::WHITE, debugTime, DEBUG_RENDER_ALWAYS );
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_8 ) ) {
		Vec3 cameraPos = m_gameCamera->GetPosition();
		std::string lightStr = "light pos is " + cameraPos.GetText();
		DebugAddWorldText( trans, Vec2( 0.5f, 0.5f ), Rgba8::RED, Rgba8::BLACK, debugTime * 2, DEBUG_RENDER_XRAY, lightStr );
	}
	
}

void Game::HandleLightKeyboardInput( float deltaSeconds )
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_COMMA ) ){
		UsePreviousShader();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_PERIOD ) ){
		UseNextShader();
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_9 ) ) {
		m_ambientLightIntensity -= 0.1f * deltaSeconds;
		if( m_ambientLightIntensity < 0.f ) {
			m_ambientLightIntensity = 0.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_0 ) ) {
		m_ambientLightIntensity += 0.1f * deltaSeconds;
		if( m_ambientLightIntensity > 1.f ) {
			m_ambientLightIntensity = 1.f;
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_T ) ){
		if( m_diffuseAttenuation.x == 1 ) {
			m_diffuseAttenuation = Vec3( 0.f, 1.f, 0.f );
		}
		else if( m_diffuseAttenuation.y == 1 ) {
			m_diffuseAttenuation = Vec3( 0.f, 0.f, 1.f );
		}
		else {
			m_diffuseAttenuation = Vec3( 1.f, 0.f, 0.f );
		}

		if( m_specularAttenuation.x == 1 ) {
			m_specularAttenuation = Vec3( 0.f, 1.f, 0.f );
		}
		else if( m_specularAttenuation.y == 1 ) {
			m_specularAttenuation = Vec3( 0.f, 0.f, 1.f );
		}
		else {
			m_specularAttenuation = Vec3( 1.f, 0.f, 0.f );
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_MINUS ) ) {
		m_lightIntensity -= 0.1f * deltaSeconds;
		if( m_lightIntensity < 0.f ) {
			m_lightIntensity = 0.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_PLUS ) ) {
		m_lightIntensity += 0.1f * deltaSeconds;
		if( m_lightIntensity > 1.0f ) {
			m_lightIntensity = 1.f;
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F5 ) ) {
		m_lightPos = Vec3::ZERO;
		m_isLightFollowCamera = false;
		m_isLightFollowAnimation = false;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F6 ) ) {
		m_lightPos = m_gameCamera->GetPosition();
		m_isLightFollowCamera = false;
		m_isLightFollowAnimation = false;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F7 ) ) {
		m_isLightFollowCamera = true;
		m_isLightFollowAnimation = false;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F8 ) ) {
		m_isLightFollowAnimation = true;
		m_isLightFollowCamera = false;
	}

	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_LEFT_BRACKET ) ) {
		m_specularFactor -= 0.1f * deltaSeconds;
		if( m_specularFactor < 0.f ) {
			m_specularFactor = 0.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_RIGHT_BRACKET ) ) {
		m_specularFactor += 0.1f * deltaSeconds;
		if( m_specularFactor > 1.0f ) {
			m_specularFactor = 1.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_SEMICOLON ) ) {
		m_specularPow -= 1.f * deltaSeconds;
		if( m_specularPow < 1.f ) {
			m_specularPow = 1.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_RIGHT_QUOTE ) ) {
		m_specularPow += 10.f * deltaSeconds;
	}

}

void Game::HandleMouseInput( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::HandleCameraMovement( float deltaSeconds )
{
	float coe = 1.0f;
	Vec3 movement = Vec3::ZERO;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_SHIFT ) ){
		coe = 2.0f;
	}
	else{
		coe = 1.0f;
	}

	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_LEFT_ARROW ) ) {
		movement.x -= 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_RIGHT_ARROW ) ) {
		movement.x += 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		movement.y += 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		movement.y -= 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ){
		movement.z += 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		movement.z -= 1.0f * coe;
	}

	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
	Vec3 rotation = Vec3( -mouseMove.y, 0.f, -mouseMove.x ) * coe * 15;
	m_gameCamera->UpdateCameraRotation( rotation );
	
	Mat44 rotationMatrix = m_gameCamera->m_transform.GetRotationMatrix();
	float temY = movement.y;
	Vec3 cameraPos = m_gameCamera->GetPosition();
	movement = rotationMatrix.TransformPosition3D( movement ); 
	movement.y = temY;
	cameraPos += movement * deltaSeconds;
	m_gameCamera->SetPosition( cameraPos );
	//g_theConsole->PrintString( Rgba8::RED, "mouse move is " + std::to_string( mouseMove.x ) + std::to_string( mouseMove.y ) );
}

void Game::UseNextShader()
{
	if( m_currentShaderIndex < m_shaderNames.size() -1 ) {
		m_currentShaderIndex++;
	}
}

void Game::UsePreviousShader()
{
	if( m_currentShaderIndex > 0 ) {
		m_currentShaderIndex--;
	}
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_X ) ){
		g_theApp->HandleQuitRequested();
	}
	if( g_theWindow->m_isWindowClose ){
		g_theApp->HandleQuitRequested();
	}
}

void Game::CheckGameStates()
{
	CheckIfPause();
	CheckIfDeveloped();
	CheckIfNoClip();
}

void Game::CheckIfPause()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ){
		m_isPause = !m_isPause;
	}
}

void Game::CheckIfDeveloped()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 )){
		m_developMode = !m_developMode;
	}
}

void Game::CheckIfNoClip()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 )){
		m_noClip = !m_noClip;
	}
}

void Game::Render() const
{
 	Rgba8 tempColor = Rgba8::DARK_GRAY;
 	m_gameCamera->m_clearColor = tempColor;
 	g_theRenderer->BeginCamera( m_gameCamera );
	g_theRenderer->SetTintColor( Rgba8::WHITE );
 	//g_theRenderer->DrawAABB2D( AABB2( Vec2( -5, -5 ),Vec2::ZERO  ), Rgba8::RED );

 	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
 	g_theRenderer->SetDiffuseTexture( temp );
	//RenderSpheres();
	//RenderCubeSphere();
	//RenderTesSpheres();


	// sd a06
	BindCurrentShader();
	RenderLightObjects();
	g_theRenderer->EndCamera( );

	DebugRenderWorldToCamera( m_gameCamera );
}

void Game::RenderLightObjects() const
{
	//g_theRenderer->RenderLight();
	g_theRenderer->SetDiffuseAttenuation( m_diffuseAttenuation );
	g_theRenderer->SetSpecularAttenuation( m_specularAttenuation );
	g_theRenderer->SetSpecularFactor( m_specularFactor );
	g_theRenderer->SetSpecularPow( m_specularPow );
	g_theRenderer->SetAmbientLight( m_ambientLightColor, m_ambientLightIntensity );
	Texture* tempNormal = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_normal.png" );
	Texture* tempDiffuse = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_diffuse.png" );
	g_theRenderer->SetDiffuseTexture( tempDiffuse );
	g_theRenderer->SetNormalTexture( tempNormal );
	m_lightQuadObject->Render();	
	m_lightCubeObject->Render();
	m_lightSphereObject->Render();
}

void Game::RenderSpheres() const
{
	for( int i = 0; i <m_sphereObjects.size(); i++ ) {
		m_sphereObjects[i]->Render();
	}
}

void Game::RenderTesSpheres() const
{
	m_tesselationObject->Render();
	
}

void Game::RenderCubeSphere() const
{
	m_cubeSphereObject->Render();
}

void Game::CreateLightQuadObjects()
{
	AABB2 QuadAABB = AABB2( Vec2( -1.5f, -1.5f ), Vec2( 1.5f, 1.5f ) );
	m_lightQuadMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForAABB2D( vertices, indices, QuadAABB, Rgba8::WHITE, Vec2::ZERO, Vec2::ONE );
	m_lightQuadMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightQuadMesh->UpdateIndicesInCPU( indices );

	m_lightQuadObject = new GameObject();
	m_lightQuadObject->m_mesh = m_lightQuadMesh;
}

void Game::CreateLightCubeObjects()
{
	AABB3 cubeAABB3 = AABB3( Vec3( -1.5f, -1.5f, -1.5f ), Vec3( 1.5f, 1.5f, 1.5f ) );
	m_lightCubeMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForAABB3D( vertices, indices, cubeAABB3, Rgba8::WHITE );
	m_lightCubeMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightCubeMesh->UpdateIndicesInCPU( indices );

	m_lightCubeObject = new GameObject();
	m_lightCubeObject->m_mesh = m_lightCubeMesh;
	m_lightCubeObject->SetPosition( Vec3( -5.f, 0.f, -5.f ) );
}

void Game::CreateLightSphereObjects()
{
	m_lightSphereMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForSphere3D( vertices, indices, Vec3::ZERO, 1.5f, 32, 16, Rgba8::WHITE );
	m_lightSphereMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightSphereMesh->UpdateIndicesInCPU( indices );

	m_lightSphereObject = new GameObject();
	m_lightSphereObject->m_mesh = m_lightSphereMesh;
	m_lightSphereObject->SetPosition( Vec3( 5.f, 0.f, -5.f ) );
}

void Game::UpdateLightObjects( float deltaSeconds )
{
	static float totalSeconds = deltaSeconds;
	totalSeconds += deltaSeconds;
	
	float x = 10.f;
	float y = 180.f;
	
	m_lightSphereObject->SetRotation( Vec3( SinDegrees( totalSeconds * 100 + x ) * y, CosDegrees( totalSeconds * 100 + x ) * y , 0.f ) );
	m_lightCubeObject->SetRotation( Vec3( SinDegrees( totalSeconds * 100 + x ) * y, CosDegrees( totalSeconds * 100 + x ) * y, 0.f ) );
}

void Game::SetAttenuation( Vec3 atten )
{
	m_diffuseAttenuation = atten;
}

void Game::SetAttenuation( float x, float y, float z )
{
	m_diffuseAttenuation = Vec3( x, y, z );
}

void Game::CreateDebugRenderObjects()
{

	DebugAddWorldPoint( Vec3( 10.f, 10.f, -10.f ), Rgba8::RED, 01.f, DEBUG_RENDER_USE_DEPTH );
	//add
	// only test for ui
	float debugTime = 10.f;
	// UI
	DebugAddScreenPoint( Vec2( 10.f, 80.f ), 20.f, Rgba8::RED, debugTime );
	//DebugAddScreenText( Vec4( 0.5f, 0.5f, -10.f, -20.f), Vec2::ZERO, 10.f, Rgba8::RED, Rgba8::GREEN, debugTime, "test" );
	DebugAddScreenQuad( AABB2( Vec2( 1.f, 1.f ), Vec2( 20.f, 20.f ) ), Rgba8::RED, debugTime );
	DebugAddScreenLine( Vec2( 15.f, 15.f ), Vec2( 40.f, 40.f ), Rgba8::BLUE, debugTime );
	DebugAddScreenArrow( Vec2( 50.f, 50.f), Vec2( 50.f, 30.f ), Rgba8::YELLOW, debugTime );
	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	DebugAddScreenTexturedQuad( AABB2( Vec2( 60.f, 60.f ), Vec2( 80.f, 80.f	) ), temp, Rgba8::GREEN, debugTime ); 

	Mat44 basisMat2D = Mat44();
	basisMat2D.SetTranslation3D( Vec3( 0.f, 0.f, -3.f ) );
	DebugAddScreenBasis( Vec2( 30.f, 30.f ), basisMat2D, Rgba8::WHITE, debugTime ); 
} 

void Game::RenderUI() const
{
	//g_theRenderer->DrawVertexVector( m_UIVertices );
}

void Game::LoadAssets()
{
	//g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	//g_theRenderer->CreateOrGetTextureFromFile("FilePath");
	//g_theAudioSystem->CreateOrGetSound("FilePath");
}

void Game::CreateTestObjects()
{
	CreateCubeSphereObjects();
	CreateSphereObjects();
	CreateLightQuadObjects();
	CreateLightCubeObjects();
	CreateLightSphereObjects();

	// tesselation object
	m_tesselationObject = new GameObject();
	m_tesMesh = new GPUMesh();
	m_tesMesh->m_owner = g_theRenderer;
	std::vector<Vertex_PCU> sphereVertices;
	std::vector<Vertex_PCU> tesIndexedVertices;
	std::vector<uint> tesIndices;
	AppendVertsForSphere3D( sphereVertices, Vec3::ZERO, 5.f, 4, 6, Rgba8::BLUE );
	AppendTesselateIndexedVerts( tesIndexedVertices, tesIndices, sphereVertices );
	m_tesMesh->UpdateVerticesInCPU( tesIndexedVertices );
	m_tesMesh->UpdateIndicesInCPU( tesIndices );
	m_tesselationObject->m_mesh = m_tesMesh;
}

void Game::CreateSphereObjects()
{
	m_sphereMesh = new GPUMesh();
	m_sphereMesh->m_owner = g_theRenderer;
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendIndexedVertsForSphere3D( vertices, indices, Vec3( 0.f, 0.f, 0.f ), 3.f, 32, 16, Rgba8::RED );
	m_sphereMesh->UpdateVerticesInCPU( vertices );
	m_sphereMesh->UpdateIndicesInCPU( indices );
	
	for( int i = 0; i < 8; i++ ){
		GameObject* meshObjects = new GameObject();
		meshObjects->m_mesh = m_sphereMesh;
		m_sphereObjects.push_back( meshObjects );
	}
}

void Game::CreateCubeSphereObjects()
{
	AABB3 cube = AABB3( Vec3::ZERO, Vec3::ONE );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendIndexedVertsForCubeSphere3D( vertices, indices, cube, 1 );
	m_cubeSphereMesh = new GPUMesh();
	m_cubeSphereMesh->m_owner = g_theRenderer;
	m_cubeSphereMesh->UpdateVerticesInCPU( vertices );
	m_cubeSphereMesh->UpdateIndicesInCPU( indices );

	m_cubeSphereObject = new GameObject();
	m_cubeSphereObject->m_mesh = m_cubeSphereMesh;


}

bool LightCommandSetAmbientColor( EventArgs& args )
{
	Vec3 color = args.GetValue( std::to_string( 0 ), Vec3::ONE );
	Vec4 v4_color = Vec4( color );
	g_theGame->m_ambientLightColor = Rgba8::GetColorFromVec4( v4_color );
	return true;
}

bool LightCommandSetLightColor( EventArgs& args )
{
	Vec3 color = args.GetValue( std::to_string( 0 ), Vec3::ONE );
	Vec4 v4_color = Vec4( color );
	g_theGame->m_lightColor = Rgba8::GetColorFromVec4( v4_color );
	return true;
}
