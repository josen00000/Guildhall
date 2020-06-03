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
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/ObjectReader.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Core/Delegate.hpp"


extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern DevConsole*		g_theConsole;
extern Game*			g_theGame;

RenderBuffer* g_dissolveBuffer;
RenderBuffer* g_projectorBuffer;
RenderBuffer* g_parallaxBuffer;
RenderBuffer* g_bloomBuffer;
RenderBuffer* g_grayBuffer;

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
	CreateGameObjects();
	LoadObjects();
	//CreateTestMaterial();
	CreateImageEffectObj();
	CreateDebugRenderObjects();

	EventCallbackFunctionPtr SetAmbientColorFuncPtr			= LightCommandSetAmbientColor;
	EventCallbackFunctionPtr SetLightColorFuncPtr			= LightCommandSetLightColor;

	g_theConsole->AddCommandToCommandList( std::string( "set_ambient_color"), std::string(" set ambient color( color = vec3)" ), SetAmbientColorFuncPtr );
	g_theConsole->AddCommandToCommandList( std::string( "set_light_color"), std::string(" set light color( color = vec3)" ), SetLightColorFuncPtr );

	g_dissolveBuffer =  new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	g_projectorBuffer =  new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	g_parallaxBuffer =  new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	g_bloomBuffer =  new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	g_grayBuffer =  new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

}

void Game::Shutdown()
{
	delete m_rng;
	delete m_sphereMesh;
	delete m_tesMesh;
	SELF_SAFE_RELEASE(m_cubeSphereMesh);
	SELF_SAFE_RELEASE(m_lightQuadMesh);
	SELF_SAFE_RELEASE(m_cubeSphereMesh);
	SELF_SAFE_RELEASE(m_lightCubeMesh);
	SELF_SAFE_RELEASE(m_lightSphereMesh);
	SELF_SAFE_RELEASE(m_loadMesh);

	SELF_SAFE_RELEASE(g_dissolveBuffer);
	SELF_SAFE_RELEASE(g_projectorBuffer);
	SELF_SAFE_RELEASE(g_parallaxBuffer);
	SELF_SAFE_RELEASE(g_bloomBuffer);
	SELF_SAFE_RELEASE(g_grayBuffer);

	//SELF_SAFE_RELEASE(m_bloomShader);
	//SELF_SAFE_RELEASE(m_grayShader);

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
	UpdateImageEffectObj();
	//UpdateLightObjects( deltaSeconds );
	UpdateUI( deltaSeconds );
	UpdateA07Objects( deltaSeconds );
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
	//g_theRenderer->EnableSpotLight( 0, m_lightPos, m_lightColor, m_lightIntensity, m_diffuseAttenuation, Vec3( 0.f, 0.f, -1.f ), 15.f, 30.f );
	g_theRenderer->EnablePointLight( 0, m_lightPos, m_lightColor, m_lightIntensity, m_diffuseAttenuation );
	//g_theRenderer->EnableDirectionLight( 0, Vec3( 0.f, 0.f, 100.f), Rgba8::RED, 1.f, Vec3( 1.f, 0.f, 0.f ) );
	//g_theRenderer->EnablePointLight( 1, Vec3::ZERO, Rgba8::BLUE, m_lightIntensity, m_diffuseAttenuation );
	//g_theRenderer->EnablePointLight( 1, Vec3::ONE, Rgba8::YELLOW, m_lightIntensity, m_diffuseAttenuation );

	// fog
	if( m_isFogEnable ) {
		g_theRenderer->SetFog( 10.f, 3.f, Rgba8::BLACK, Rgba8::RED );
	}
	else {
		g_theRenderer->DisableFog();
	}
}

void Game::UpdateA07Objects( float deltaSeocnds )
{
	// dissolve object
	float burnOutTime = 10.f;
	static float totalSeconds = 0.0f;
	//Rgba8
	Rgba8 startColor = Rgba8::RED;
	Rgba8 endColor = Rgba8::WHITE;
	totalSeconds += deltaSeocnds;
	static dissolve_data_t  dissolve_data;
	dissolve_data.burnAmount = totalSeconds / burnOutTime;
	dissolve_data.burnStartColor = startColor.GetVec3Color();
	dissolve_data.burnEndColor = endColor.GetVec3Color();
	dissolve_data.burnEdgeWidth = 0.3f;
	if( dissolve_data.burnAmount > 1 ){
		dissolve_data.burnAmount = 1;
	}
	
	g_dissolveBuffer->Update( &dissolve_data, sizeof( dissolve_data_t), sizeof(dissolve_data_t) );

	// update project object
	UpdateProjectObjects( deltaSeocnds );

	// update larallax box
	static parallax_data_t parallax_data;
	parallax_data.depth = m_parallaxDepth;
	parallax_data.count = 8;
	//parallax_data.depth = 0.05f;

	g_parallaxBuffer->Update( &parallax_data, sizeof( parallax_data_t), sizeof( parallax_data_t) );
}

void Game::UpdateProjectBuffer()
{
	static project_data_t project_data;
	Mat44 view =  m_gameCamera->GetUpdatedViewMatrix();
	Mat44 projection = Mat44::CreatePerspectiveProjectionMatrix( 30.f, 1.f, -0.1f, -100.f );
	projection.Multiply( view );
	project_data.world_to_clip = projection;
	project_data.pos = m_gameCamera->GetPosition();
	project_data.power = 1.f;
	g_projectorBuffer->Update( &project_data, sizeof(project_data_t), sizeof(project_data_t) );
	//g_theRenderer->SetMaterialBuffer(g_projectorBuffer);
}

void Game::CreateShaderNames()
{
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
	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 7 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "button 'z' project lock" + GetStringFromBool( !m_isprojectFollowCamera ) ));
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 8 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"parallax depth(has bug): %.2f. ['N' , 'M'] ", m_parallaxDepth );
	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 9 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "button 'f' disable, enable fog. using lit shader" + std::to_string( g_theRenderer->GetTotalTexturePoolCount() ) ));
	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 10 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "Total Texture Pool count" + std::to_string( g_theRenderer->GetTotalTexturePoolCount()) ));
	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 11 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "Texture Pool Free count" + std::to_string( g_theRenderer->GetTexturePoolFreeCount()) ));

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
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_Z ) ) {
		m_isprojectFollowCamera = !m_isprojectFollowCamera;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F ) ) {
		m_isFogEnable = !m_isFogEnable;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_B ) ) {
		m_isUsingBloomEffect = !m_isUsingBloomEffect;
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_G ) ) {
		m_isUsingGrayEffect = !m_isUsingGrayEffect;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_N ) ) {
		m_tintStrength += 1.f * deltaSeconds;
		if( m_tintStrength > 1.f ) {
			m_tintStrength = 1.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_M ) ) {
		m_tintStrength -= 1.f * deltaSeconds;
		if( m_tintStrength < 0.f ) {
			m_tintStrength = 0.f;
		}
	}

	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_J ) ) {
		m_grayStrength += 1.f * deltaSeconds;
		if( m_grayStrength > 1.f ) {
			m_grayStrength = 1.f;
		}
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_K ) ) {
		m_grayStrength -= 1.f * deltaSeconds;
		if( m_grayStrength < 0.f ) {
			m_grayStrength = 0.f;
		}
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

	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
		movement.x -= 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		movement.x += 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		movement.y += 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		movement.y -= 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ){
		movement.z -= 1.0f * coe;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		movement.z += 1.0f * coe;
	}

	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
	Vec3 rotation = Vec3( -mouseMove.y, 0.f, -mouseMove.x ) * coe * 15;
	m_gameCamera->UpdateCameraRotation( rotation );
	
	Mat44 rotationMatrix = m_gameCamera->m_transform.GetRotationMatrix( PITCH_ROLL_YAW_ORDER );
	float temY = movement.y;
	movement.y = 0.0f;
	Vec3 cameraPos = m_gameCamera->GetPosition();
	movement = rotationMatrix.TransformPosition3D( movement ); 
	movement.y += temY;
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
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	Texture* grayScaleTex		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* bloomTex			= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* colorTarget		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* normalTarget		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* bloomTarget		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* albedoTarget		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	Texture* tangentTarget		= g_theRenderer->AcquireRenderTargetMatching( backBuffer );

	//m_gameCamera->SetColorTarget( colorTarget, 0 );
	//m_gameCamera->SetColorTarget( bloomTarget, 1 );
	//m_gameCamera->SetColorTarget( normalTarget, 2 );
	//m_gameCamera->SetColorTarget( albedoTarget, 3 );
	//m_gameCamera->SetColorTarget( tangentTarget, 4 );


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


	// sd a07
	BindCurrentShader();
	//RenderLightObjects();
	//RenderA07Objects();
	// sd a08
	Rendera08Objects();
	g_theRenderer->EndCamera( );

	DebugRenderWorldToCamera( m_gameCamera );
	DebugRenderScreenTo( m_gameCamera->GetColorTarget() );
	// image effect
	// sd a09
// 	if( m_isUsingGrayEffect ) {
// 		g_grayBuffer->Update( &m_colorData, sizeof(Mat44), sizeof(Mat44) );
// 		//g_theRenderer->BindUniformBuffer( 6, g_grayBuffer);
// 		//g_theRenderer->SetMaterialBuffer( g_grayBuffer );
// 		TestImageEffect( grayScaleTex, colorTarget, m_grayShader, g_grayBuffer );
// 	}
// 	if( m_isUsingBloomEffect ) {
// 		
// 		float data[4];
// 		data[0] = (float)bloomTarget->GetTexelSize().x;
// 		data[1] = (float)bloomTarget->GetTexelSize().y;
// 		data[2] = 0.f;
// 		data[3] = 0.f;
// 
// 		g_bloomBuffer->Update( &data, sizeof(ColorData), sizeof(ColorData) );
// 		g_theRenderer->SetMaterialBuffer( g_bloomBuffer );
// 		TestBloomImageEffect( bloomTex, colorTarget, bloomTarget, m_bloomShader, g_bloomBuffer );
// 	}
// 	//TestImageEffect( tempRT );
// 
// 	//GUARANTEE_OR_DIE( g_theRenderer->m_totalRenderTargetMade < 10, "leaking " );
// 	
// 	// setup camera 
// 	// bind shader for effect
// 	// render full screen image
// 	
// 	if( m_isUsingBloomEffect ) {
// 		g_theRenderer->CopyTexture( backBuffer, bloomTex );
// 	}
// 	else if( m_isUsingGrayEffect ) {
// 		g_theRenderer->CopyTexture( backBuffer, grayScaleTex );
// 	}
// 	else {
// 		g_theRenderer->CopyTexture( backBuffer, colorTarget );
// 	}

	g_theRenderer->ReleaseRenderTarget( colorTarget );
	g_theRenderer->ReleaseRenderTarget( normalTarget );
	g_theRenderer->ReleaseRenderTarget( bloomTarget );
	g_theRenderer->ReleaseRenderTarget( albedoTarget );
	g_theRenderer->ReleaseRenderTarget( tangentTarget );
	g_theRenderer->ReleaseRenderTarget( grayScaleTex );
	g_theRenderer->ReleaseRenderTarget( bloomTex );
}

void Game::RenderLightObjects() const
{
	//g_theRenderer->RenderLight();
	//g_theRenderer->SetDiffuseTexture()
	g_theRenderer->SetCullMode(RASTER_CULL_NONE);
	g_theRenderer->SetDiffuseAttenuation( 0, m_diffuseAttenuation );
	g_theRenderer->SetSpecularAttenuation( 0, m_specularAttenuation );
	g_theRenderer->SetSpecularFactor( m_specularFactor );
	g_theRenderer->SetSpecularPow( m_specularPow );
	g_theRenderer->SetAmbientLight( m_ambientLightColor, m_ambientLightIntensity );
	//g_theRenderer->BindShader( "data/Shader/fresnel.hlsl" ); // testing
	Texture* tempNormal = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_normal.png" );
	Texture* tempDiffuse = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_diffuse.png" );
	g_theRenderer->SetDiffuseTexture( tempDiffuse );
	g_theRenderer->SetNormalTexture( tempNormal );
	m_lightQuadObject->Render();

	// fresnel object
	g_theRenderer->BindShader( "Data/Shader/fresnel.hlsl" );
	//m_lightCubeObject->Render();
	m_lightSphereObject->Render();
}

void Game::RenderA07Objects() const
{
	// dissolve object
	// bind texture
	// bind ubo buffer
	// render
	g_theRenderer->SetCullMode(RASTER_CULL_BACK);
	g_theRenderer->BindShader("Data/Shader/dissolve.hlsl");
	g_theRenderer->SetMaterialBuffer( g_dissolveBuffer );
	Texture* disTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/noise.png" );
	g_theRenderer->SetMaterialTexture( disTex );
	m_dissolveObject->Render();

	// triplanar object
	g_theRenderer->BindShader("Data/Shader/triplanar.hlsl");
	Texture* triDifTex1 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/dry_rock/color.jpg" );
	Texture* triDifTex2 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/example_colour.png" );
	Texture* triDifTex3 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ruin_wall_03_0.png" );
	Texture* triNorTex1 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/dry_rock/normal.jpg" );
	Texture* triNorTex2 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/example_normal.png" );
	Texture* triNorTex3 = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images//ruin_wall_03_norm_0.png" );
	g_theRenderer->SetDiffuseTexture( triDifTex1, 1 );
	g_theRenderer->SetDiffuseTexture( triDifTex2, 2 );
	g_theRenderer->SetDiffuseTexture( triDifTex3, 3 );
	g_theRenderer->SetNormalTexture( triNorTex1, 1 );
	g_theRenderer->SetNormalTexture( triNorTex2, 2 );
	g_theRenderer->SetNormalTexture( triNorTex3, 3 );
	m_triplanarObject->Render();
	// light
// 	g_theRenderer->BindShader( "Data/Shader/lit.hlsl" );
// 	g_theRenderer->SetDiffuseTexture( triDifTex1 );
// 	g_theRenderer->SetNormalTexture( triNorTex1 );
// 	m_lightQuadObject->Render();	



	// project object
	g_theRenderer->BindShader( "Data/Shader/project.hlsl" );
	g_theRenderer->SetMaterialBuffer( g_projectorBuffer );
	Texture* projectTestTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/chi.jpg" );
	g_theRenderer->SetDiffuseTexture( projectTestTex );
	for( int i = 0; i < 5; i++ ) {
		m_projectObjects[i]->Render();
	}

	// parallax object
	g_theRenderer->BindShader( "Data/Shader/parallax.hlsl" );
	g_theRenderer->SetMaterialBuffer( g_parallaxBuffer );
	Texture* parallaxTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ruin_wall_03_height_0.png" );
	g_theRenderer->SetMaterialTexture( parallaxTex );
	g_theRenderer->SetDiffuseTexture( triDifTex3 );
	g_theRenderer->SetNormalTexture( triNorTex3 );
	m_parallaxObject->Render();

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
	m_lightQuadMesh->m_debugMsg = "lightquad";
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
	m_lightCubeMesh->m_debugMsg = "lightcube";
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
	m_lightSphereMesh->m_debugMsg = "light sphere";
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForSphere3D( vertices, indices, Vec3::ZERO, 1.5f, 32, 16, Rgba8::WHITE );
	m_lightSphereMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightSphereMesh->UpdateIndicesInCPU( indices );

	m_lightSphereObject = new GameObject();
	m_lightSphereObject->m_mesh = m_lightSphereMesh;
	m_lightSphereObject->SetPosition( Vec3( 3.5f, 0.f, -5.f ) );
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

void Game::CreateDissolveObject()
{
	m_dissolveObject = new GameObject();
	m_dissolveObject->m_mesh = m_lightCubeMesh;
	m_dissolveObject->SetPosition( Vec3( -5.f, 0.f, -5.f ) );
}

void Game::CreateTriPlanarObjects()
{
	m_triplanarObject = new GameObject();
	m_triplanarObject->m_mesh = m_lightSphereMesh;
	m_triplanarObject->SetPosition( Vec3( 7.f, 0.f, -5.f ) );
	m_triplanarObject->SetRotation( Vec3::ZERO );
}

void Game::CreateProjectObjects()
{
	Vec3 startPos( -10.f, 5.f, -10.f );
	for( int i = 0; i < 5; i++ ) {
		GameObject* tempObj = new GameObject();
		tempObj->m_mesh = m_lightCubeMesh;
		tempObj->SetPosition( startPos + Vec3( 3.f * i, 0.f, 0.f ) );
		tempObj->SetRotation( Vec3::ZERO );
		m_projectObjects.push_back( tempObj );
	}
}

void Game::CreateParallaxObjects()
{
	Vec3 startPos( 5.f, 7.f, -5.f );
	m_parallaxObject = new GameObject();
	m_parallaxObject->m_mesh = m_lightCubeMesh;
	m_parallaxObject->SetPosition( startPos );
}

void Game::UpdateProjectObjects( float deltaSeconds )
{
	static float totalTime = 0.f;
	totalTime += deltaSeconds;
	float rotDegrees = SinDegrees( totalTime * 30.f ) * 30.f ;
	for( int i = 0; i < 5; i++ ) {
		m_projectObjects[i]->SetRotation( Vec3( 0.f, 0.f, rotDegrees * i ) ); 
	}
	if( m_isprojectFollowCamera ) {
		UpdateProjectBuffer();
	}
	Transform trans = *(m_projectObjects[0]->m_trans);
	Vec3 pos = trans.GetPosition();
	pos += Vec3( 0.f, 3.f, 0.f );
	trans.SetPosition( pos );
	DebugAddWorldText( trans, Vec2::ZERO, Rgba8::RED, Rgba8::RED, 0.1f, DEBUG_RENDER_ALWAYS, std::string("projector shader") );
}

void Game::LoadObjects()
{
	ObjectReader* testObjReader = new ObjectReader( "Data/Model/luka/mesh.obj" );
	//testObjReader->SetPostTransRot( Vec3( 0.f, 0.f, 180.f ) );
	m_loadObject = new GameObject();
	m_loadMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	testObjReader->GenerateGPUMesh( *m_loadMesh );
	m_loadObject->m_mesh = m_loadMesh;
	m_loadObject->SetPosition( Vec3( 0.f, 0.f, -10.f ) );
	Texture* temTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Model/luka/diffuse.png" );
	m_loadObject->m_tex = temTex; 

	m_testShaderState = new ShaderState( g_theRenderer, "Data/Shader/States/defaultShader.xml" );
}

void Game::Updatea08Objects( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::Rendera08Objects() const
{
	g_theRenderer->DisableFog();
	//g_theRenderer->BindShader( nullptr );
	g_theRenderer->BindShaderState( m_testShaderState );
	//g_theRenderer->BindMaterial( m_testMaterial );
	//g_theRenderer->SetFillMode( RASTER_FILL_WIREFRAME );
	m_loadObject->Render();
}

void Game::TestImageEffect( Texture* dstTex, Texture* srcTex, Shader* shader, RenderBuffer* ubo ) const
{
	g_theRenderer->StartEffect( dstTex, srcTex, shader, ubo );
	g_theRenderer->EndEffect();
}

void Game::TestBloomImageEffect( Texture* dstTex, Texture* normalColor, Texture* bloomTex, Shader* shader, RenderBuffer* ubo ) const
{
	g_theRenderer->StartBloomEffect( dstTex, bloomTex, normalColor, shader, ubo );
	g_theRenderer->EndEffect();
}

void Game::CreateTestMaterial()
{
	m_testMaterial = new Material();
	m_testMaterial->SetShaderState( m_testShaderState );
	Texture* temTex = g_theRenderer->CreateOrGetTextureFromFile( "Data/Model/halloween_miku/diffuse.png" );
	m_testMaterial->AddTexture( temTex );

	Rgba8 startColor = Rgba8::RED;
	Rgba8 endColor = Rgba8::WHITE;
	dissolve_data_t  dissolve_data;
	dissolve_data.burnAmount = 1;
	dissolve_data.burnStartColor = startColor.GetVec3Color();
	dissolve_data.burnEndColor = endColor.GetVec3Color();
	dissolve_data.burnEdgeWidth = 0.3f;
	if( dissolve_data.burnAmount > 1 ) {
		dissolve_data.burnAmount = 1;
	}
	//m_testMaterial->SetData<dissolve_data_t>(  &dissolve_data );
	//g_dissolveBuffer->Update( &dissolve_data, sizeof( dissolve_data_t ), sizeof( dissolve_data_t ) );

}

void Game::CreateImageEffectObj()
{
	m_tintStrength = 0.f;
	m_grayStrength = 1.f;
	m_targetColor = Rgba8::RED;
	Vec3 v3_color =  m_tintStrength * m_targetColor.GetVec3Color();
	Mat44 tintMat = Mat44(
		Vec3( 1 - m_tintStrength, 0, 0 ),
		Vec3( 0, 1 - m_tintStrength, 0 ),
		Vec3( 0, 0, 1 - m_tintStrength ),
		v3_color
		);
	Mat44 grayMat = Mat44(
 		Vec3( 1 - 0.66f * m_grayStrength, 0.33f * m_grayStrength, 0.33f * m_grayStrength ),
 		Vec3( 0.33f * m_grayStrength, 1 - 0.66f * m_grayStrength, 0.33f * m_grayStrength ),
 		Vec3( 0.33f * m_grayStrength, 0.33f * m_grayStrength, 1 - 0.66f * m_grayStrength ),
 		Vec3::ZERO
 		);
	tintMat.Multiply( grayMat );
	m_colorData.colorMat = tintMat;



	m_grayShader = g_theRenderer->GetOrCreateShader( "Data/SHader/GrayEffect.hlsl" );
	m_bloomShader = g_theRenderer->GetOrCreateShader( "Data/Shader/BloomEffect.hlsl" );
}

void Game::UpdateImageEffectObj()
{
	Vec3 v3_color =  m_tintStrength * m_targetColor.GetVec3Color();
	Mat44 tintMat = Mat44(
		Vec3( 1 - m_tintStrength, 0, 0 ),
		Vec3( 0, 1 - m_tintStrength, 0 ),
		Vec3( 0, 0, 1 - m_tintStrength ),
		v3_color
		);
	Mat44 grayMat = Mat44(
		Vec3( 1 - 0.66f * m_grayStrength, 0.33f * m_grayStrength, 0.33f * m_grayStrength ),
		Vec3( 0.33f * m_grayStrength, 1 - 0.66f * m_grayStrength, 0.33f * m_grayStrength ),
		Vec3( 0.33f * m_grayStrength, 0.33f * m_grayStrength, 1 - 0.66f * m_grayStrength ),
		Vec3::ZERO
		);
	tintMat.Multiply( grayMat );
	m_colorData.colorMat = tintMat;
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

void Game::CreateGameObjects()
{
	CreateCubeSphereObjects();
	CreateSphereObjects();
	CreateLightQuadObjects();
	CreateLightCubeObjects();
	CreateLightSphereObjects();
	CreateDissolveObject();
	CreateTriPlanarObjects();
	CreateProjectObjects();
	CreateParallaxObjects();

	// tesselation object
	m_tesselationObject = new GameObject();
	m_tesMesh = new GPUMesh();
	m_tesMesh->m_debugMsg = "tesMesh";
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
	m_sphereMesh->m_debugMsg = "sphere mesh";
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
	m_cubeSphereMesh->m_debugMsg = "my cube sphere ";
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
