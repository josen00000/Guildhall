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
	CreateTestObjects();
	//CreateDebugRenderObjects();
	//DebugAddScreenPoint( Vec2( 10.f, 80.f ), 20.f, Rgba8::RED, 20.f );
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_sphereMesh;
	delete m_tesMesh;
	SELF_SAFE_RELEASE(m_lightQuadMesh);
	SELF_SAFE_RELEASE(m_cubeSphereMesh);
	SELF_SAFE_RELEASE(m_lightCubeMesh);


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
	UpdateLighting();
	UpdateLightObjects( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateLighting()
{
	Vec3 lightPos = m_gameCamera->GetPosition();
// 	std::string lightStr = "light pos is " + lightPos.GetText();
// 	DebugAddScreenText( Vec4( 0.1f, 0.1f, 0.f, 0.f ), Vec2::ZERO, 20.f, Rgba8::BLACK, Rgba8::BLACK, 0.f, lightStr );
// 	DebugAddScreenText( Vec4( 0.5f, 0.5f, -11.f, -20.f), Vec2::ZERO, 10.f, Rgba8::RED, Rgba8::GREEN, 0.f, "test" );
// 	Transform testTrans;
// 	testTrans.SetPosition( Vec3( -1.f, -1.f, -1.f ) );
// 
// 	std::string lightStr1 = lightStr + " world testing!";
// 	DebugAddWorldText( testTrans, Vec2::ZERO, Rgba8::BLACK, Rgba8::BLACK, 0.f, DEBUG_RENDER_ALWAYS, lightStr1 );
	//lightPos = Vec3( 1.f, 1.f, -3.f );
	g_theRenderer->EnablePointLight( 0, lightPos, Rgba8::GREEN, 1.f, Vec3::ZERO );
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
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_9 ) ) {
		Vec3 cameraPos = m_gameCamera->GetPosition();
		std::string lightStr = "light pos is " + cameraPos.GetText();
		DebugAddWorldBillboardText( trans.m_pos, Vec2( 0.5f, 0.5f ), Rgba8::RED, Rgba8::GREEN, debugTime * 2, DEBUG_RENDER_ALWAYS, lightStr );
	}

	
}

void Game::HandleLightKeyboardInput( float deltaSeconds )
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_COMMA ) ){
		g_theRenderer->UsePreviousShader();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_PERIOD ) ){
		g_theRenderer->UseNextShader();
		g_theRenderer->BindShader( "data/Shader/surface_normals.hlsl" );
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
 	g_theRenderer->DrawAABB2D( AABB2( Vec2( -5, -5 ),Vec2::ZERO  ), Rgba8::RED );
 	//g_theRenderer->BindShader( "data/Shader/Reverse.hlsl" );
 	//g_theRenderer->DrawAABB2D( AABB2( Vec2::ZERO, Vec2(5,5) ), Rgba8::RED );

 	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
 	g_theRenderer->SetDiffuseTexture( temp );
	//RenderSpheres();
	//RenderCubeSphere();
	//RenderTesSpheres();

	RenderLightObjects();
	g_theRenderer->EndCamera( );

	DebugRenderWorldToCamera( m_gameCamera );
}

void Game::RenderLightObjects() const
{
	//g_theRenderer->RenderLight();
	g_theRenderer->SetSpecularFactor( 0.8f );
	g_theRenderer->SetSpecularPow( 32.f );
	g_theRenderer->SetAmbientLight( Rgba8::BLUE, 1.f );
	//g_theRenderer->BindTexture("tile_normal)
	Texture* tempNormal = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_normal.png" );
	Texture* tempDiffuse = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/tile_diffuse.png" );
	g_theRenderer->SetDiffuseTexture( tempDiffuse );
	g_theRenderer->SetNormalTexture( tempNormal );
	//g_theRenderer->BindShader( "data/Shader/surface_normals.hlsl" );
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
	AABB2 QuadAABB = AABB2( Vec2( 0.f, 0.f), Vec2( 3.f, 3.f ) );
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
	AABB3 cubeAABB3 = AABB3( Vec3( -5.f, 0.f, -6.5f ), Vec3( -2.f, 3.f, -3.5f ) );
	m_lightCubeMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForAABB3D( vertices, indices, cubeAABB3, Rgba8::WHITE );
	m_lightCubeMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightCubeMesh->UpdateIndicesInCPU( indices );

	m_lightCubeObject = new GameObject();
	m_lightCubeObject->m_mesh = m_lightCubeMesh;
}

void Game::CreateLightSphereObjects()
{
	m_lightSphereMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCUTBN );
	std::vector<Vertex_PCUTBN> vertices;
	std::vector<uint> indices;
	AppendIndexedTBNVertsForSphere3D( vertices, indices, Vec3( 7.5f, 1.5f, -5.f ), 1.5f, 32, 16, Rgba8::WHITE );
	m_lightSphereMesh->UpdateTBNVerticesInCPU( vertices );
	m_lightSphereMesh->UpdateIndicesInCPU( indices );

	m_lightSphereObject = new GameObject();
	m_lightSphereObject->m_mesh = m_lightSphereMesh;
}

void Game::UpdateLightObjects( float deltaSeconds )
{

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

