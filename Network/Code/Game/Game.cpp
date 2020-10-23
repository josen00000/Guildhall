#include "Game.hpp"
#include <string.h>
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Game/World.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileMap.hpp"
#include "Game/MaterialDefinition.hpp"
#include "Game/RegionDefinition.hpp"
#include "Game/TestJob.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Network/NetworkSystem.hpp"
#include "Engine/Network/UDPSocket.hpp"
#include "Engine/Network/TCPClient.hpp"
#include "Engine/Network/TCPServer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Job/JobSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/ObjectReader.hpp"
#include "Engine/Renderer/ShaderState.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Sampler.hpp"


extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern InputSystem*		g_theInputSystem;
extern NetworkSystem*	g_theNetworkSystem;
extern JobSystem*		g_theJobSystem;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;
extern Game*			g_theGame;


// static void AddDebugMessage( std::string msg ) {
// 	static int index = 0;
// 	//DebugAddScreenText( Vec4( 0.f, 0.f, 50.f, index * 3.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.f, msg );
// }

Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_gameState = GAME_STATE_LOADING;
	m_debugMode		= false;
	m_ghostMode		= false;
	m_isPause		= false;
	m_debugCamera	= false;
	m_isAppQuit		= false;
	m_isAttractMode	= false;

	// debug render system
	DebugRenderSystemStartup( g_theRenderer, m_gameCamera );
	LoadAssets();
	CreateGameObjects();
	//JobTest();

	// create world
	m_player = new Entity( EntityDefinition::s_definitions["Player"] );
	m_player->SetIsPlayer( true );
	m_world = new World();
	std::string startMapName = g_gameConfigBlackboard.GetValue( "StartMap", "" );
	LoadMapsDefinitionsAndCreateMaps( "Data/Maps" );

	m_world->LoadMap( startMapName );
	Map* currentMap = m_world->GetCurrentMap();
	//currentMap->m_actors.push_back( m_player );
	// command
	g_theConsole->AddCommandToCommandList( std::string( "load_map" ), std::string( "Load one map with name." ), MapCommandLoadMap );
	g_theConsole->AddCommandToCommandList( std::string( "client_connect" ), std::string( "Connect to target server" ), ConnectTo );
	g_theConsole->AddCommandToCommandList( std::string( "send_message" ), std::string( "Connect to target server" ), SendMessageTest );
	g_theConsole->AddCommandToCommandList( std::string( "start_server" ), std::string( "start server for listening" ), StartServer );
	g_theConsole->AddCommandToCommandList( std::string( "send_data" ), std::string( "send Data to client" ), SendData );
	g_theConsole->AddCommandToCommandList( std::string( "stop_server" ), std::string( "stop the server" ), StopServer );
	g_theConsole->AddCommandToCommandList( std::string( "disconnect" ), std::string( "disconnect the server" ), Disconnect );
	g_theConsole->AddCommandToCommandList( std::string( "open_udp_port" ), std::string( "bind and open udp socket" ), OpenUDPPort );
	g_theConsole->AddCommandToCommandList( std::string( "send_udp_msg" ), std::string( "send udp msg" ), SendUDPMessage );
	g_theConsole->AddCommandToCommandList( std::string( "close_udp_port" ), std::string( "bind and open udp socket" ), CloseUDPPort );

	m_billBoardEntity = new Entity();
 	m_billBoardEntity->m_cylinder = Cylinder3( Vec3::ZERO, Vec3( 0.f, 0.f, 1.f ), 0.5f );

	// network start up
	m_testClient = g_theNetworkSystem->CreateClient();
	m_testServer = g_theNetworkSystem->CreateServer();
}

void Game::Shutdown()
{
	delete m_rng;
	SELF_SAFE_RELEASE(m_world);
	SELF_SAFE_RELEASE(m_cubeMesh);
	SELF_SAFE_RELEASE(m_viewModelSpriteSheet);
	DebugRenderSystemShutdown();
}

void Game::Reset()
{
	Startup();
}

void Game::Update( float deltaSeconds )
{
	m_DebugDeltaSeconds = deltaSeconds;
	HandleInput( deltaSeconds );
	//UpdateLighting( deltaSeconds );
	UpdateUI( deltaSeconds );
	m_world->Update( deltaSeconds );

	UpdatePlayer( deltaSeconds );
	//UpdateBillboardTest();

	g_theJobSystem->ClaimAndDeleteAllCompletedJobs();

	//RaycastTest( Vec3::ZERO, Vec3::ZERO, 1.f );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateLighting( float deltaSeconds )
{
	UNUSED(deltaSeconds);
// 	static float animationSeconds = 0.f;
// 	float theta = animationSeconds * 30.f;
// 	float dist = 8.f;
// 	if( m_isLightFollowCamera ) {
// 		m_lightPos = m_gameCamera->GetPosition();
// 	}
// 	else {
// 		DebugAddWorldPoint( m_lightPos, 1.f, m_lightColor, 0.1f, DEBUG_RENDER_ALWAYS );
// 	}
// 	if( m_isLightFollowAnimation ) {
// 		animationSeconds += deltaSeconds;
// 		m_lightPos = Vec3( CosDegrees( theta ) * dist, 0, -SinDegrees( theta ) * dist ) + Vec3( 0.f, 0.f, -5.f );
// 	}
// 	else {
// 		animationSeconds = 0.f;
// 	}
// 	//g_theRenderer->EnableSpotLight( 0, m_lightPos, m_lightColor, m_lightIntensity, m_diffuseAttenuation, Vec3( 0.f, 0.f, -1.f ), 15.f, 30.f );
// 	g_theRenderer->EnablePointLight( 0, m_lightPos, m_lightColor, m_lightIntensity, m_diffuseAttenuation );
// 	//g_theRenderer->EnableDirectionLight( 0, Vec3( 0.f, 0.f, 100.f), Rgba8::RED, 1.f, Vec3( 1.f, 0.f, 0.f ) );
// 	//g_theRenderer->EnablePointLight( 1, Vec3::ZERO, Rgba8::BLUE, m_lightIntensity, m_diffuseAttenuation );
// 	//g_theRenderer->EnablePointLight( 1, Vec3::ONE, Rgba8::YELLOW, m_lightIntensity, m_diffuseAttenuation );
// 
// 	// fog
// 	if( m_isFogEnable ) {
// 		g_theRenderer->SetFog( 10.f, 3.f, Rgba8::BLACK, Rgba8::RED );
// 	}
// 	else {
// 		g_theRenderer->DisableFog();
// 	}
}

void Game::UpdateBillboardTest()
{
	m_billBoardEntity->UpdateTexCoords( m_gameCamera, m_billboardMode, m_convension );
	m_billBoardEntity->UpdateVerts( Vec2::ZERO, Vec2::ONE );
}

void Game::RenderDebugInfo() const
{
	float index = 1;
	const Transform cameraTrans = m_gameCamera->m_transform;
	const Vec3 cameraPos = cameraTrans.GetPosition();
	Mat44 cameraModelMat = cameraTrans.ToMatrix( m_convension );

	
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2, Vec2::ZERO, Rgba8::WHITE, "Camera Pitch: %.2f, Roll: %.2f, Yaw: %.2f", cameraTrans.GetPitchDegrees(), cameraTrans.GetRollDegrees(), cameraTrans.GetYawDegrees() );
	index++;
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2, Vec2::ZERO, Rgba8::WHITE, "Camera Pos X: %.2f, Roll: %.2f, Yaw: %.2f", cameraPos.x, cameraPos.y, cameraPos.z );
	index++;
	Vec3 iBasis = cameraModelMat.GetIBasis3D();
	DebugAddScreenLeftAlignTextf( 1.f,  -index * 2, Vec2::ZERO, Rgba8::WHITE, "IBasis: %.2f, %.2f, %.2f", iBasis.x, iBasis.y, iBasis.z );
	index++;
	Vec3 jBasis = cameraModelMat.GetJBasis3D();
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2 , Vec2::ZERO, Rgba8::WHITE, "JBasis: %.2f, %.2f, %.2f", jBasis.x, jBasis.y, jBasis.z );
	index++;
	Vec3 kBasis = cameraModelMat.GetKBasis3D();
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2 , Vec2::ZERO, Rgba8::WHITE, "KBasis: %.2f, %.2f, %.2f", kBasis.x, kBasis.y, kBasis.z );
	index++;
 	std::string billBoardMsg;
	switch( m_billboardMode )
	{
	case BILLBOARD_MODE_CAMERA_FACING_XY:
		billBoardMsg = "CAMERA FACING XY";
		break;
	case BILLBOARD_MODE_CAMERA_OPPOSING_XY:
		billBoardMsg = "CAMERA OPPOSING XY";
		break;
	case BILLBOARD_MODE_CAMERA_FACING_XYZ:
		billBoardMsg = "CAMERA FACING XYZ";
		break;
	case BILLBOARD_MODE_CAMERA_OPPOSING_XYZ:
		billBoardMsg = "CAMERA OPPOSING XYZ";
		break;
	case NUM_BILLBOARD_MODES:
		break;
	default:
		break;
	}
	DebugAddScreenLeftAlignText( 1.f, -index * 2, Vec2::ZERO, Rgba8::WHITE, 0.f, billBoardMsg );
	index++;
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2 , Vec2::ZERO, Rgba8::WHITE, "waiting jobs: %i", g_theJobSystem->GetUndoJobsNum() );
	index++;
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2 , Vec2::ZERO, Rgba8::WHITE, "runninging jobs: %i", g_theJobSystem->GetRunningJobsNum() );
	index++;
	DebugAddScreenLeftAlignTextf( 1.f, -index * 2 , Vec2::ZERO, Rgba8::WHITE, "completed jobs: %i", g_theJobSystem->GetCompletedJobsNum() );

}

void Game::RenderFPSInfo() const
{
	int ms = (int)( m_DebugDeltaSeconds * 1000 );
	int FPS = (int)( 1.0f / m_DebugDeltaSeconds );
	DebugAddScreenRightAlignTextf( 1.f, -2.f, Vec2::ZERO, Rgba8::WHITE, "ms/frame = %d ( %.d FPS)\n", ms, FPS );
	//DebugAddScreenLine( Vec2( 110.f, 50.f), Vec2( 140.f, 50.f ), Rgba8::RED, 0.f );
}

void Game::RenderBillboardTest() const
{
	g_theRenderer->SetCullMode( RASTER_CULL_BACK );
	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->SetDiffuseTexture( temp );
	g_theRenderer->DrawVertexVector( m_billBoardEntity->m_verts );
	DebugAddWorldArrow( m_billBoardEntity->GetPosition(), m_billBoardEntity->GetPosition() + ( m_billBoardEntity->m_texForward * 2.f ), Rgba8::RED, 0.0f, DEBUG_RENDER_ALWAYS );
	//DebugAddWorldPoint( m_billBoardEntity->GetPosition(), 3.f, Rgba8::RED, 0.f, DEBUG_RENDER_ALWAYS );
}


void Game::RaycastTest( Vec3 startPos, Vec3 forwardNormal, float maxDist )
{
	Map* currentMap = m_world->GetCurrentMap();
		Vec3 startPos1 = Vec3( 3.57f, 3.87f, 0.5f );
		Vec3 forwardNormal1 = Vec3( -0.087f, 0.938f, -0.334f );
// 	static Vec3 startPos1 = startPos;
// 	static Vec3 forwardNormal1 = forwardNormal;
	forwardNormal1.Normalize();
	static float maxDist1 = maxDist;
	currentMap->RayCast( startPos1, forwardNormal1, maxDist1 );
}

void Game::JobTest()
{
	for( int i = 0; i < 20; i++ ) {
		TestJob* testJob = new TestJob();
		//g_theJobSystem->PostJob( testJob );
	}
}

void Game::UpdatePlayer( float delteSeconds )
{
// 	static Vec3 testPos = Vec3::ZERO;
// 	static Vec3 forward = Vec3::ZERO;
// 	static float maxDist = 0.f;
// 	if( m_player ) {
// 		testPos = m_player->GetPosition();
// 		forward =m_gameCamera->GetForwardDirt( m_convension );
// 		maxDist = 1.f;
// 	}
// 	//RaycastTest( testPos, forward, maxDist );
// 	if( !m_player ){ return; }
// 
// 	float baseRotSpeed = 30.f;
// 	float playerOrientation = m_player->m_orientation;
// 
// 	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
// 	float deltaPlayerOrientation = -mouseMove.x * baseRotSpeed;
// 
// 	playerOrientation += deltaPlayerOrientation;
// 	m_gameCamera->m_transform.SetYawDegrees( playerOrientation );
// 
// 	bool isMoving = false;
// 	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
// 		isMoving = true;
// 		m_player->SetMoveDirt( Vec2( 1.f, 0.f ) );
// 	}
// 	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
// 		isMoving = true;
// 		m_player->SetMoveDirt( Vec2( -1.f, 0.f ) );
// 	}
// 	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_F ) ) {
// 		isMoving = true;
// 		m_player->SetMoveDirt( Vec2( 0.f, -1.f ) );
// 	}
// 	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
// 		isMoving = true;
// 		m_player->SetMoveDirt( Vec2( 0.f, 1.f ) );
// 	}
// 	m_player->SetIsMoving( isMoving );
// 	m_player->SetOrientation( playerOrientation );
// 	m_player->Update( delteSeconds );
// 
// 	m_gameCamera->SetPosition( m_player->GetPosition() );
// 
// 	Map* currentMap = m_world->GetCurrentMap();
// 	currentMap->RayCast( m_player->GetPosition() + m_gameCamera->GetForwardDirt( m_convension ) * m_player->GetRadius() * 1.5f, m_gameCamera->GetForwardDirt( m_convension ), 1.f );
}

void Game::HandleInput( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() ) {
		HandleKeyboardInput( deltaSeconds );
		HandleAudioKeyboardInput();
		HandleCameraMovement( deltaSeconds );
	}
}

// void Game::CreateDebugScreen()
// {
// 	float deltaHeight = 3.f;
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f ), Vec2::ZERO, 3.f, Rgba8::WHITE, "Ambient light intensity: %.2f. [9,0]", m_ambientLightIntensity );
// 	std::string attenuationText = "Attenuation: " + m_diffuseAttenuation.GetText();
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Diffuse Attenuation: %.2f, %.2f, %.2f ", m_diffuseAttenuation.x, m_diffuseAttenuation.y, m_diffuseAttenuation.z );
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 2 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Attenuation: %.2f, %.2f, %.2f ", m_specularAttenuation.x, m_specularAttenuation.y, m_specularAttenuation.z );
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 3 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Light intensity: %.2f. [-,+] ", m_lightIntensity );
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 4 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Factor: %.2f. ['[',']'] ", m_specularFactor );
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 5 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"Specular Pow: %.2f. [';' , '''] ", m_specularPow );
// 	std::string shaderName = "Shader : " + m_shaderNames[m_currentShaderIndex];
// 	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 6 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, shaderName );
// 	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 7 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "button 'z' project lock" + GetStringFromBool( !m_isprojectFollowCamera ) ));
// 	DebugAddScreenTextf( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 8 ), Vec2::ZERO, 3.f, Rgba8::WHITE ,"parallax depth(has bug): %.2f. ['N' , 'M'] ", m_parallaxDepth );
// 	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 9 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "button 'f' disable, enable fog. using lit shader" + std::to_string( g_theRenderer->GetTotalTexturePoolCount() ) ));
// 	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 10 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "Total Texture Pool count" + std::to_string( g_theRenderer->GetTotalTexturePoolCount()) ));
// 	DebugAddScreenText( Vec4( 0.f, 1.f, 5.f, -5.f - deltaHeight * 11 ), Vec2::ZERO, 3.f, Rgba8::WHITE, Rgba8::WHITE, 0.000001f, std::string( "Texture Pool Free count" + std::to_string( g_theRenderer->GetTexturePoolFreeCount()) ));
// 
// }

void Game::HandleKeyboardInput( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	CheckIfExit();
	CheckGameStates();
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_PLUS ) ) {
		if( m_billboardMode < NUM_BILLBOARD_MODES ) {
			m_billboardMode = (BillboardMode)(m_billboardMode + 1);
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_MINUS ) ) {
		if( m_billboardMode > 0  ) {
			m_billboardMode = (BillboardMode)(m_billboardMode - 1);
		}
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_J ) ) {
		g_theJobSystem->Shutdown();
	}

}

void Game::HandleCameraMovement( float deltaSeconds )
{
	float baseMoveSpeed = 5.f;
	float baseRotSpeed = 30.f;
	Vec3 movement = Vec3::ZERO;
	Mat44 rotMat = m_gameCamera->m_transform.GetRotationMatrix( m_convension );
	Vec3 forward = rotMat.GetIBasis3D();
	Vec3 forwardXY0 = Vec3( forward.x, forward.y, 0.f );
	Vec3 left = rotMat.GetJBasis3D();
	Vec3 leftXY0 = Vec3( left.x, left.y, 0.f );

	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_E ) ) {
		movement += forwardXY0 * baseMoveSpeed * deltaSeconds ;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		movement -= forwardXY0 * baseMoveSpeed * deltaSeconds;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_F ) ) {
		movement -= leftXY0 * baseMoveSpeed * deltaSeconds;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {

		movement += leftXY0 * baseMoveSpeed * deltaSeconds;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
		movement += Vec3( 0.f, 0.f, 1.f ) * baseMoveSpeed * deltaSeconds;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_R ) ) {
		movement -= Vec3( 0.f, 0.f, 1.f ) * baseMoveSpeed * deltaSeconds;
	}

	Vec3 cameraPos = m_gameCamera->GetPosition();
	cameraPos += movement;
	if( m_ghostMode ) {
		m_gameCamera->SetPosition( cameraPos );
	}

	Vec2 mouseMove = g_theInputSystem->GetRelativeMovementPerFrame();
	float deltaCameraYaw = -mouseMove.x * baseRotSpeed ;
	float deltaCameraPitch = mouseMove.y * baseRotSpeed ;

	float cameraPitch = m_gameCamera->m_transform.GetPitchDegrees();
	float cameraYaw = m_gameCamera->m_transform.GetYawDegrees();

	cameraPitch += deltaCameraPitch;
	cameraYaw += deltaCameraYaw;
	cameraPitch = ClampFloat( -89.9f, 89.9f, cameraPitch );
	
	m_gameCamera->m_transform.SetPitchDegrees( cameraPitch );
	m_gameCamera->m_transform.SetYawDegrees( cameraYaw );
}

void Game::HandleAudioKeyboardInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ) {
		PlayerTestSound();
	}
}

void Game::PlayerTestSound()
{
	SoundID testSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	//g_theAudioSystem->PlaySound( testSound, false, m_rng->RollRandomFloatInRange( 0.5f, 1.f ), m_rng->RollRandomFloatInRange( -1.f, 1.f ), m_rng->RollRandomFloatInRange( 0.5f, 2.f ) );
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
	//CheckIfGhost();
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
		m_debugMode = !m_debugMode;
		m_world->SetDebugMode( m_debugMode );
	}
}

void Game::CheckIfGhost()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 )){
		m_ghostMode = !m_ghostMode;
		if( m_ghostMode ) {
			m_player->SetIsPlayer( false );
			m_player = nullptr;
		}
		else {
			PossessEntityAsPlayer();
		}
	}
}



void Game::RenderGame() const
{
	return;
 	Rgba8 tempColor = Rgba8::DARK_GRAY;
 	m_gameCamera->m_clearColor = tempColor;
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	m_gameCamera->SetColorTarget( backBuffer, 0 );


 	g_theRenderer->BeginCamera( m_gameCamera, m_convension );
	g_theRenderer->SetBlendMode( BlendMode::BLEND_ALPHA );
	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetCullMode( RASTER_CULL_BACK );
	g_theRenderer->SetTintColor( Rgba8::WHITE );
 	g_theRenderer->BindShader( "Data/Shader/WorldOpaque.hlsl" );
	
	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );

 	g_theRenderer->SetDiffuseTexture( temp );

	m_world->Render();

	g_theRenderer->SetCullMode( RASTER_CULL_NONE );
	g_theRenderer->DisableDepth();
	g_theRenderer->BindShader( static_cast<Shader*>(nullptr) );
	g_theRenderer->SetDiffuseTexture( nullptr );

	if( m_debugMode ) {
		RenderBasis();
	}

	g_theRenderer->EndCamera( );

	RenderUI();

	DebugRenderWorldToCamera( m_gameCamera, m_convension );
	DebugRenderScreenTo( m_gameCamera->GetColorTarget() );
}

void Game::RenderUI() const
{
	if( m_debugMode ) {
		RenderDebugInfo();
	}
	RenderFPSInfo();
	
	m_UICamera->SetDepthStencilTarget( nullptr ); // don't need depth stencil target set target to nullptr
	m_UICamera->SetClearMode( CLEAR_NONE );
	m_UICamera->SetUseDepth( false );
	m_UICamera->SetColorTarget( m_gameCamera->GetColorTarget() );
	//Sampler* uiSampler = new Sampler( g_theRenderer, SAMPLER_POINT );
	//g_theRenderer->BindSampler( uiSampler );
	g_theRenderer->BeginCamera( m_UICamera, X_RIGHT_Y_UP_Z_BACKWARD );

	RenderBaseHud();
	RenderGun();
	
	g_theRenderer->EndCamera();
	//delete uiSampler;

}

void Game::RenderBasis() const
{
	Transform basisTrans;
	basisTrans.SetPosition( Vec3::ZERO );
	basisTrans.SetPitchDegrees( 0.f );
	basisTrans.SetRollDegrees( 0.f );
	basisTrans.SetYawDegrees( 0.f );
	DebugAddWorldBasis( basisTrans.ToMatrix( m_convension ), Rgba8::WHITE, Rgba8::WHITE, 0.01f, DEBUG_RENDER_ALWAYS );

	Transform compassTrans;
	Vec3 cameraPos = m_gameCamera->GetPosition();
	Vec3 cameraForward = m_gameCamera->m_transform.GetForwardDirt( m_convension );
	compassTrans.SetPosition( cameraPos + cameraForward * 0.1f );
	compassTrans.SetScale( Vec3( 0.01f, 0.01f, 0.01f ) );
	DebugAddWorldBasis( compassTrans.ToMatrix( m_convension ), Rgba8::WHITE, Rgba8::WHITE, 0.f, DEBUG_RENDER_ALWAYS );
}

void Game::RenderBaseHud() const
{
	Texture* hudTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Hud_Base.png" );
	g_theRenderer->SetDiffuseTexture( hudTexture );
	AABB2 hudBox = AABB2( Vec2::ZERO, Vec2( 160.f, 11.7f ) );
	g_theRenderer->DrawAABB2D( hudBox, Rgba8::WHITE );
}

void Game::RenderGun() const
{
	Texture* gunTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );
	g_theRenderer->SetDiffuseTexture( gunTexture );
	Vertex_PCU vertices[6];
	Vec2 gunUVAtMax;
	Vec2 gunUVAtMin;
	m_viewModelSpriteSheet->GetSpriteUVs( gunUVAtMin, gunUVAtMax ,IntVec2( 0, 0 ) );
	vertices[0] = Vertex_PCU( Vec3( 55.f, 11.7f, 0.f), Rgba8::WHITE, gunUVAtMin );
	vertices[1] = Vertex_PCU( Vec3( 105.f, 11.7f, 0.f), Rgba8::WHITE, Vec2( gunUVAtMax.x, gunUVAtMin.y ) );
	vertices[2] = Vertex_PCU( Vec3( 105.f, 61.7f, 0.f), Rgba8::WHITE, gunUVAtMax );

	vertices[3] = Vertex_PCU( Vec3( 55.f, 11.7f, 0.f), Rgba8::WHITE, gunUVAtMin );
	vertices[4] = Vertex_PCU( Vec3( 105.f, 61.7f, 0.f), Rgba8::WHITE, gunUVAtMax );
	vertices[5] = Vertex_PCU( Vec3( 55.f, 61.7f, 0.f), Rgba8::WHITE, Vec2( gunUVAtMin.x, gunUVAtMax.y ) );

	g_theRenderer->DrawVertexArray( 6, vertices );
}

void Game::SetConvention( Convention convention )
{
	m_convension = convention;
}

void Game::SetCameraPos( Vec3 pos )
{
	m_gameCamera->SetPosition( pos );
}

void Game::SetCameraYaw( float yaw )
{
	m_gameCamera->SetYawRotation( yaw );
}

Texture* Game::GetMapTexture()
{
	return MaterialDefinition::s_sheet.m_diffuseTexture;
}

std::vector<Vertex_PCU>& Game::GetMapRenderData()
{
	TileMap* currentMap = dynamic_cast<TileMap*>( m_world->GetCurrentMap() );
	return currentMap->m_meshes;
}

SpriteSheet* Game::GetViewModelSpriteSheet()
{
	return m_viewModelSpriteSheet;
}

void Game::LoadAssets()
{
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/Teleporter.wav" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png");
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png");
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Hud_Base.png" );
	Texture* modelSpriteSheet = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/ViewModelsSpriteSheet_8x8.png" );
	m_viewModelSpriteSheet = new SpriteSheet( modelSpriteSheet, IntVec2( 8 ) );

	LoadDefinitions();
}

void Game::LoadDefinitions()
{
	// load material def
	g_theConsole->DebugLogf( "Parsing map materials from \"%s\"...", "Data/Definitions/MapMaterialTypes.xml"   );
	LoadFileDefinition( MaterialDefinition::LoadMapMaterialDefinitions, "Data/Definitions/MapMaterialTypes.xml" );
	g_theConsole->DebugLogf( "Parsing map region types from \"%s\"...", "Data/Definitions/MapRegionTypes.xml"   );
	LoadFileDefinition( RegionDefinition::LoadMapRegionDefinitions, "Data/Definitions/MapRegionTypes.xml" );
	g_theConsole->DebugLogf( "Parsing entity types from \"%s\"...", "Data/Definitions/MapRegionTypes.xml"   );
	LoadFileDefinition( EntityDefinition::LoadEntityDefinitions, "Data/Definitions/EntityTypes.xml" );
}

void Game::LoadMapsDefinitionsAndCreateMaps( const char* mapFolderPath )
{
	g_theConsole->DebugLogf( "Loading 5 map files from \"%s\"",mapFolderPath );
	Strings mapPaths = GetFileNamesInFolder( mapFolderPath, "*.xml" );
	for( int i = 0; i < mapPaths.size(); i++ ) {
		XmlDocument mapFile;
		char mapFilePath[100];
// 		if( mapPaths[i].compare( "TestRoom.xml" ) != 0 ) {
// 			continue;
// 		}
		strcpy_s( mapFilePath, mapFolderPath );
		strcat_s( mapFilePath, "/" );
		strcat_s( mapFilePath, mapPaths[i].c_str() );
		mapFile.LoadFile( mapFilePath );
		const XmlElement* mapElement = mapFile.RootElement();
		
		g_theConsole->DebugLogf( "Loading map files from \"%s\"", mapPaths[i].c_str() );

		std::string mapType = ParseXmlAttribute( *mapElement, "type", "TileMap" );
		Map* tempMap = nullptr;
		if( mapType == "TileMap" )	{ tempMap = new TileMap( *mapElement, mapFilePath, m_world ); }

		tempMap->m_name = mapPaths[i];
		if( tempMap ) {
			m_world->AddMap( tempMap );
		}
	}

}

bool Game::LoadMapWithName( std::string mapName )
{
	//std::string pureMapname = std::string( mapName, 0, mapName.size()-4 );
	if( m_world->LoadMap( mapName ) ) {
		g_theConsole->DebugLogf( "Entering map \"%s\"", mapName.c_str() );
		return true;
	}
	
	return false;
}

Strings Game::GetAllMaps() const
{
	return m_world->GetAllMaps();
}

void Game::CreateGameObjects()
{
	AABB3 cube = AABB3( Vec3::ZERO, Vec3::ONE );
	m_cubeMesh = new GPUMesh( g_theRenderer, VERTEX_TYPE_PCU );
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;


	AppendIndexedVertsForAABB3D( vertices, indices, cube, Rgba8::WHITE, m_convension );
	m_cubeMesh->UpdateVerticesInCPU( vertices );
	m_cubeMesh->UpdateIndicesInCPU( indices );

	m_cubeObj = new GameObject();
	m_cubeObj1 = new GameObject();
	m_cubeObj2 = new GameObject();

	m_cubeObj->SetMesh( m_cubeMesh );
	m_cubeObj1->SetMesh( m_cubeMesh );
	m_cubeObj2->SetMesh( m_cubeMesh );

	m_cubeObj->SetPosition( Vec3( 2.f, 0.f, 0.f ) );
	m_cubeObj1->SetPosition( Vec3( 0.f, 2.f, 0.f ) );
	m_cubeObj2->SetPosition( Vec3( 2.f, 2.f, 0.f ) );

	// debug render
	Mat44 basisMat = Mat44::IDENTITY;
	basisMat.SetTranslation3D( Vec3( 0, 5.f, 0.f ) );
}


void Game::PossessEntityAsPlayer()
{
// 	Map* currentMap = m_world->GetCurrentMap();
// 	Vec3 cameraPos = m_gameCamera->GetPosition();
// 	Vec3 cameraForward = m_gameCamera->GetForwardDirt( m_convension );
// 	Vec2 cameraPosXY = cameraPos.GetXYVector();
// 	Vec2 cameraForwardXY = cameraForward.GetXYVector();
// 	float dist = 2.f;
// 	Entity* tempPlayer = nullptr;
// 
// 	std::vector<Entity*> mapEntities = currentMap->m_actors;
// 	for( int i = 0; i < mapEntities.size(); i++ ) {
// 		Entity* tempEntity = mapEntities[i];
// 		Vec2 entityPosXY = tempEntity->Get2DPosition();
// 		Vec2 cameraToEntityXY = entityPosXY - cameraPosXY;
// 		Vec3 cameraToEntity = tempEntity->GetPosition() - cameraPos;
// 		float angleDegrees = GetAngleDegreesBetweenVectors2D( cameraToEntityXY, cameraForwardXY );
// 		if( angleDegrees < 45 ) {
// 			float cameraToEntityDist = cameraToEntity.GetLength();
// 			if( cameraToEntityDist < dist ) {
// 				dist = cameraToEntityDist;
// 				tempPlayer = tempEntity;
// 			}
// 		}
// 	}
// 	if( tempPlayer ) {
// 		m_player = tempPlayer;
// 		m_player->SetIsPlayer( true );
// 	}
// 	else {
// 		m_ghostMode = true;
// 	}
}

// command
bool MapCommandLoadMap( EventArgs& args )
{
	std::string mapName = args.GetValue( std::to_string( 0 ), "" );
	if( !g_theGame->LoadMapWithName( mapName ) ) {
		Strings mapNames = g_theGame->GetAllMaps();
		g_theConsole->DebugLog( mapNames );
		std::string exampleMapName = std::string( mapNames[0], 0, mapNames[0].size() - 4 );
		
		std::string mapExample( "For example: load_map: " + exampleMapName );
		g_theConsole->DebugLog( mapExample );
	}
	return true;
}

bool MapCommandWarp( EventArgs& args )
{
// 	IntVec2 startPos = args.GetValue( std::to_string( 0 ), IntVec2::ZERO );
// 	float startYaw = args.GetValue( std::to_string( 0 ), 0.f );

	//g_theGame->SetStartPos();
	//g_theGame->SetStartYaw();
	UNUSED(args);
	return true;
}

bool ConnectTo( EventArgs& args )
{
	TCPClient* tempClient = g_theNetworkSystem->m_client;
	std::string targetIPAddr = args.GetValue( std::to_string( 0 ), "" );
	std::string targetPort	= args.GetValue( std::to_string( 1 ), "" );

	tempClient->ConnectToServerWithIPAndPort( targetIPAddr.c_str(), targetPort.c_str() );
	g_theNetworkSystem->m_isServer = false;
	return true;
}

bool SendMessageTest( EventArgs& args )
{
	TCPClient* tempClient = g_theNetworkSystem->m_client;
	TCPServer* tempServer = g_theNetworkSystem->m_server;
	if( !g_theNetworkSystem->m_isServer ) {
		std::string message = args.GetValue( std::to_string( 0 ), "" );
		tempClient->SetSendData( message.data(), (int)message.size() );
	}
	else {
		std::string message = args.GetValue( std::to_string( 0 ), "" );
		tempServer->SetSendData( message.data(), (int)message.size() );
	}
	return true;
}

bool StartServer( EventArgs& args )
{
	std::string port = args.GetValue( std::to_string( 0 ), "" );
	g_theNetworkSystem->StartServerWithPort( port.c_str() );
	if( !g_theNetworkSystem->m_isServer ) {
		g_theNetworkSystem->m_isServer = true;
	}
	return true;
}

bool SendData( EventArgs& args )
{
	std::string data = args.GetValue( std::to_string( 0 ), "" );
	TCPServer* test = g_theNetworkSystem->m_server;
	test->SetSendData( data.c_str(), (int)data.size()  );
	
	return true;
}

bool StopServer( EventArgs& args )
{
	g_theNetworkSystem->m_server->ShutDown();
	return true;
}

bool Disconnect( EventArgs& args )
{
	g_theNetworkSystem->m_client->DisconnectServer();
	return true;
}

bool OpenUDPPort( EventArgs& args )
{
	std::string bindportNum = args.GetValue( std::to_string( 0 ), "" );
	std::string sendportNum = args.GetValue( std::to_string( 1 ), "" );
	g_theNetworkSystem->m_UDPSocket->CreateUDPSocket( sendportNum.c_str(), "192.168.1.206" );
	//g_theNetworkSystem->m_UDPSocket->CreateUDPSocket( sendportNum.c_str(), "10.8.155.124" );
	g_theNetworkSystem->m_UDPSocket->BindSocket( atoi(bindportNum.c_str()) );
	g_theConsole->DebugLogf( "open success" );
	return true;
}

bool SendUDPMessage( EventArgs& args )
{
	std::string msg = args.GetValue( std::to_string( 0 ), "" );
	g_theNetworkSystem->SetSendMsg( msg );
	return true;
}

bool CloseUDPPort( EventArgs& args )
{
	UNUSED(args);
	g_theNetworkSystem->CloseUDPSocket();
	return true;
}
