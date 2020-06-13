#include "Game.hpp"
#include <string.h>
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Game/World.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/MaterialDefinition.hpp"
#include "Game/RegionDefinition.hpp"
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
	m_noClip		= false;
	m_isPause		= false;
	m_debugCamera	= false;
	m_isAppQuit		= false;
	m_isAttractMode	= false;

	// debug render system
	DebugRenderSystemStartup( g_theRenderer, m_gameCamera );
	LoadAssets();
	CreateGameObjects();
	// create world
	m_world = new World();
}

void Game::Shutdown()
{
	delete m_rng;
	SELF_SAFE_RELEASE(m_world);
	SELF_SAFE_RELEASE(m_cubeMesh);
	DebugRenderSystemShutdown();
}

void Game::RunFrame( float deltaSeconds )
{
	m_DebugDeltaSeconds = deltaSeconds;
	Update( deltaSeconds );
	HandleInput( deltaSeconds );
}

void Game::Reset()
{
	Startup();
}

void Game::Update( float deltaSeconds )
{
	UpdateLighting( deltaSeconds );
	UpdateUI( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	//DebugAddScreenText( Vec4( 0.5f, 0.5f, 0.f, 0.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 0.1f, std::string( "camera Pos" + m_gameCamera->GetPosition().ToString() ) );
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
}

void Game::RenderFPSInfo() const
{
	int ms = (int)( m_DebugDeltaSeconds * 1000 );
	int FPS = (int)( 1.0f / m_DebugDeltaSeconds );
	DebugAddScreenRightAlignTextf( 1.f, -2.f, Vec2::ZERO, Rgba8::WHITE, "ms/frame = %d ( %.d FPS)\n", ms, FPS );
	//DebugAddScreenLine( Vec2( 110.f, 50.f), Vec2( 140.f, 50.f ), Rgba8::RED, 0.f );
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
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_debugMode = !m_debugMode;
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
	m_gameCamera->SetPosition( cameraPos );

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
	g_theAudioSystem->PlaySound( testSound, false, m_rng->RollRandomFloatInRange( 0.5f, 1.f ), m_rng->RollRandomFloatInRange( -1.f, 1.f ), m_rng->RollRandomFloatInRange( 0.5f, 2.f ) );
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
		m_debugMode = !m_debugMode;
	}
}

void Game::CheckIfNoClip()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 )){
		m_noClip = !m_noClip;
	}
}

void Game::RenderGame() const
{
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

	m_cubeObj->Render();
	m_cubeObj1->Render();
	m_cubeObj2->Render();

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

void Game::SetConvention( Convention convention )
{
	m_convension = convention;
}

void Game::LoadAssets()
{
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/TestSound.mp3" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/Teleporter.wav" );
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png");
	g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png");

	LoadDefinitions();
	LoadMaps( "Data/Maps" );
}

void Game::LoadDefinitions()
{
	// load material def
	LoadFileDefinition( MaterialDefinition::LoadMapMaterialDefinitions, "Data/Definitions/MapMaterialTypes.xml" );
	LoadFileDefinition( RegionDefinition::LoadMapRegionDefinitions, "Data/Definitions/MapRegionTypes.xml" );
	LoadFileDefinition( EntityDefinition::LoadEntityDefinitions, "Data/Definitions/EntityTypes.xml" );
}

void Game::LoadMaps( const char* mapFolderPath )
{
	Strings mapPaths = GetFileNamesInFolder( mapFolderPath, "*.xml" );
	for( int i = 0; i < mapPaths.size(); i++ ) {
		XmlDocument mapFile;
		char mapFilePath[100];
		strcpy_s( mapFilePath, mapFolderPath );
		strcat_s( mapFilePath, "/" );
		strcat_s( mapFilePath, mapPaths[i].c_str() );
		mapFile.LoadFile( mapFilePath );
		const XmlElement* mapElement = mapFile.RootElement();
		MapDefinition mapDef = MapDefinition( *mapElement );
		MapDefinition::s_definitions[mapPaths[i]] = mapDef;
	}
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
