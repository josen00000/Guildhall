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
	DebugRenderSystemStartup( g_theRenderer );
	CreateTestObjects();
	CreateDebugRenderObjects();
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_cubeMesh;
	delete m_sphereMesh;
	delete m_tesMesh;
	SELF_SAFE_RELEASE(m_cubeSphereMesh);

	// clear game objects
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
	UNUSED(deltaSeconds);
	if( !g_theConsole->IsOpen() ) {
		HandleKeyboardInput( deltaSeconds );
	}
	UpdateMeshes( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateMeshes( float deltaSeconds )
{
	static float totalSeconds = 0.f;
	totalSeconds += deltaSeconds;
	float rotCoe = SinDegrees( totalSeconds );
	Vec3 rotation = Vec3( 0, 0, rotCoe * 180 );
	m_cubeMesh->SetRotation( rotation );

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

void Game::HandleKeyboardInput( float deltaSeconds )
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

void Game::HandleMouseInput()
{
	
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
 	g_theRenderer->DrawAABB2D( AABB2( Vec2( -5, -5 ),Vec2::ZERO  ), Rgba8::RED );
 	//g_theRenderer->BindShader( "data/Shader/Reverse.hlsl" );
 	//g_theRenderer->DrawAABB2D( AABB2( Vec2::ZERO, Vec2(5,5) ), Rgba8::RED );
 	g_theRenderer->SetModelMatrix( m_cubeMesh->GetModelMatrix() );
 	g_theRenderer->DrawMesh( m_cubeMesh );

 	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
 	g_theRenderer->BindTexture( temp );
	//RenderSpheres();
	//RenderCubeSphere();
	//RenderTesSpheres();
	g_theRenderer->EndCamera( );

	DebugRenderWorldToCamera( m_gameCamera );
	DebugRenderScreenTo( m_gameCamera->GetColorTarget() );
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

void Game::CreateDebugRenderObjects()
{
	// world
	//DebugAddWorldPoint( Vec3( 0, 0, -10.f ), Rgba8::RED, 20.f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldLine( Vec3( 0.f, 0.f, -10.f ), Rgba8::RED, Rgba8::RED,  Vec3( -5.f, 0.f, -10.f ), Rgba8::BLUE, Rgba8::BLUE, 3.f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldArrow( Vec3( 0.f, 0.f, -10.f ), Rgba8::RED, Rgba8::RED,  Vec3( 0.f, 3.f, -10.f ), Rgba8::BLUE, Rgba8::BLUE, 3.f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldLine( Vec3( 0.f, 0.f, -10.f ), Rgba8::RED, Rgba8::RED,  Vec3( 0.f, 10.f, -10.f ), Rgba8::BLUE, Rgba8::BLUE, 3.f, DEBUG_RENDER_ALWAYS );
	
	// basis
// 	Mat44 basisMat = Mat44();
// 	basisMat.SetTranslation3D( Vec3( 0.f, 0.f, -3.f ) );
// 	DebugAddWorldBasis( basisMat, Rgba8::RED, Rgba8::BLUE, 5.f, DEBUG_RENDER_ALWAYS );

	//DebugAddWorldQuad( Vec3( 0.f, 0.f, -5.f), Vec3( 3.f, 0.f, -5.f ), Vec3( 3.f, 5.f, -5.f ), Vec3( 0.f, 5.f, -5.f ), Rgba8::BLUE, Rgba8::BLACK, 3.f, DEBUG_RENDER_ALWAYS );

	// box
	//AABB3 box = AABB3( Vec3( -1.5f, -3.f, -5.5f ), Vec3( 1.5f, 3.f, -14.5f ));
	//DebugAddWorldWireBounds( box, Vec3( 0.f, 1.f, 0.f), Rgba8::RED, Rgba8::BLUE, 3.f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldWireSphere( Vec3( 0.f, 0.f, -10.f), 10.f, Rgba8::RED, Rgba8::BLUE, 3.f, DEBUG_RENDER_ALWAYS );

	Transform textTrans;
	textTrans.SetPosition( Vec3 ( 0.f, 0.f, -10.f) );
	textTrans.SetRotationFromPitchRollYawDegrees( Vec3( 0.f, 0.f, 0.f ) );
	textTrans.SetScale( Vec3::ONE );
	std::string textString = std::string( "testing!!!" ); 
	Transform textTrans1 = textTrans;
	textTrans1.SetPosition( Vec3 ( 0.f, 0.3f, -11.f ) );
	//DebugAddWorldText( textTrans, Vec2( 0.5f, 0.5f ), Rgba8::RED, Rgba8::BLACK, 20.f, DEBUG_RENDER_USE_DEPTH, textString );
	//DebugAddWorldText( textTrans1, Vec2( 0.5f, 0.5f ), Rgba8::GREEN, Rgba8::BLACK, 20.f, DEBUG_RENDER_XRAY, textString );
	//DebugAddWorldBillboardText( Vec3( 0.f, 1.f, -10.f ), Vec2( 0.5f, 0.5f ), Rgba8::RED, Rgba8::GREEN, 20.f, DEBUG_RENDER_USE_DEPTH, textString );


	// UI
	DebugAddScreenPoint( Vec2( 10.f, 10.f ), 20.f, Rgba8::RED, 20.f );
	DebugAddScreenQuad( AABB2( Vec2( 1.f, 1.f ), Vec2( 20.f, 20.f ) ), Rgba8::RED, 20.f );
	DebugAddScreenLine( Vec2( 15.f, 15.f ), Vec2( 40.f, 40.f ), Rgba8::BLUE, 10.f );
	//DebugAddScreenArrow( Vec2( 50.f, 50.f), Vec2( 40.f, 40.f ), Rgba8::YELLOW, 20.f );
	//Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );

	//DebugAddScreenTexturedQuad( AABB2( Vec2( 60.f, 60.f ), Vec2( 80.f, 80.f	) ), temp, Rgba8::GREEN, 10.f ); 
	//DebugAddScreenText( Vec4( 0.5f, 0.5f, -10.f, -20.f), Vec2::ZERO, 10.f, Rgba8::RED, Rgba8::GREEN, 10.f, "shit!!!!!" );


	Mat44 basisMat2D = Mat44();
	basisMat2D.SetTranslation3D( Vec3( 0.f, 0.f, -3.f ) );
	//DebugAddScreenBasis( Vec2( 30.f, 30.f ), basisMat2D, Rgba8::WHITE, 20.f ); 
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
	m_cubeMesh = new GPUMesh();
	m_cubeMesh->m_owner = g_theRenderer;
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AABB2 testAABB = AABB2( Vec2::ZERO, Vec2::ONE * 10 );
	AABB3 testAABB3 = AABB3( Vec3::ZERO, Vec3::ONE );
	m_cubeMesh->SetPosition( Vec3( 1.f, 0.5f, -12.f ) );
	AppendIndexedVertsForAABB3D( vertices, indices, testAABB3, Rgba8::BLUE );
	m_cubeMesh->UpdateVerticesInCPU( vertices );
	m_cubeMesh->UpdateIndicesInCPU( indices );

	CreateCubeSphereObjects();
	CreateSphereObjects();

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

