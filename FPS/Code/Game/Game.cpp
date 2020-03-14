#include "Game.hpp"
#include "Game/App.hpp"
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
	CreateTestObjects();

}

void Game::Shutdown()
{
	delete m_rng;
	delete m_cubeMesh;
	delete m_sphereMesh;
	delete m_tesMesh;

	// clear game objects
	for( int i = 0; i < m_sphereObjects.size(); i++ ){
		delete m_sphereObjects[i];
	}
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
		HandleKeyboardInput();
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

void Game::HandleKeyboardInput()
{
	CheckIfExit();
	HandleCameraMovement();
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

void Game::HandleCameraMovement()
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
	Vec3 rotation = Vec3( -mouseMove.y, 0.f, -mouseMove.x ) * coe * 10;
	m_gameCamera->UpdateCameraRotation( rotation );
	
	Mat44 rotationMatrix = m_gameCamera->m_transform.GetRotationMatrix();
	float temY = movement.y;
	Vec3 cameraPos = m_gameCamera->GetPosition();
	movement = rotationMatrix.TransformPosition3D( movement ); 
	movement.y = temY;
	cameraPos += movement;
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
	//m_world->Render(); past code
	Rgba8 tempColor = Rgba8::GRAY;
	m_gameCamera->m_clearColor = tempColor;
	g_theRenderer->BeginCamera( *m_gameCamera );
	g_theRenderer->DrawAABB2D( AABB2( Vec2( -5, -5 ),Vec2::ZERO  ), Rgba8::RED );
	//g_theRenderer->BindShader( "data/Shader/Reverse.hlsl" );
	//g_theRenderer->DrawAABB2D( AABB2( Vec2::ZERO, Vec2(5,5) ), Rgba8::RED );
	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->BindTexture( temp );
	g_theRenderer->SetModelMatrix( m_cubeMesh->GetModelMatrix() );
	g_theRenderer->DrawMesh( m_cubeMesh );

	RenderSpheres();
	RenderTesSpheres();
	g_theRenderer->EndCamera( );

	
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
	AppendIndexedVertsForAABB3D( vertices, indices, testAABB3, Rgba8::RED );
	m_cubeMesh->UpdateVerticesInCPU( vertices );
	m_cubeMesh->UpdateIndicesInCPU( indices );

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

