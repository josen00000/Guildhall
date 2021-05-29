#include "game.hpp"
#include "Game/App.hpp"
#include "../../Thesis/Code/Game/Camera/CameraSystem.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/World.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"

//////////////////////////////////////////////////////////////////////////
// global variable
//////////////////////////////////////////////////////////////////////////
// Game
//////////////////////////////////////////////////////////////////////////
extern App*				g_theApp;
extern CameraSystem*	g_theCameraSystem;
extern Game*			g_theGame;
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//#define DEBUG_MODE
// Engine
//////////////////////////////////////////////////////////////////////////
extern InputSystem*		g_theInputSystem;
extern BitmapFont*		g_squirrelFont;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;	

//////////////////////////////////////////////////////////////////////////
// public
//////////////////////////////////////////////////////////////////////////

// Constructor
//////////////////////////////////////////////////////////////////////////
Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
}
//////////////////////////////////////////////////////////////////////////


// Game Flow
//////////////////////////////////////////////////////////////////////////
void Game::Startup()
{
	LoadAssets();
	LoadDefs();
	g_theCameraSystem = new CameraSystem();
	g_theCameraSystem->Startup();
	//SetZoomSetting();
	m_world = World::CreateWorld( 2 );

	g_theConsole->AddCommandToCommandList( std::string( "set_asymptotic"), std::string( "Asymptotic value for camera controller" ), SetCameraAsymptoticValue );
	g_theConsole->AddCommandToCommandList( std::string( "spawn_item"), std::string( "spawn new item" ), SpawnItem );
	g_theConsole->AddCommandToCommandList( std::string( "delete_player"), std::string( "delete player with index" ), DeletePlayer );
	g_theConsole->AddCommandToCommandList( std::string( "set_camera_smooth" ), std::string( "set is controller smooth motion" ), SetIsSmooth );
	g_theConsole->AddCommandToCommandList( std::string( "load_map" ), std::string( "load map" ), LoadMap );
}

void Game::Shutdown()
{
	delete m_gameCamera;
	delete m_UICamera;

	m_gameCamera	= nullptr;
	m_UICamera		= nullptr;
}

void Game::RunFrame( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() ) {
		HandleInput();
	}
	if( m_isPaused ) {
		deltaSeconds = 0.f;
	}
	UpdateGame( deltaSeconds );
	if( m_useUI ) {
		//UpdateUI( );
	}

}

void Game::RenderGame( int controllerIndex ) const
{
	m_world->RenderWorld( controllerIndex );
}

void Game::RenderUI() const
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}

void Game::EndFrame()
{
	m_world->EndFrame();
}

//////////////////////////////////////////////////////////////////////////


Map* Game::GetCurrentMap()
{
	return m_world->GetCurrentMap();
}

// Mutator
//////////////////////////////////////////////////////////////////////////
void Game::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
	g_theCameraSystem->SetIsDebug( isDebug );
}

void Game::SetIsPaused( bool isPaused )
{
	m_isPaused = isPaused;
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////

// Game flow
//////////////////////////////////////////////////////////////////////////
void Game::HandleInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		bool isdebug = GetIsDebug();
		SetIsDebug( !isdebug );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F2 ) ) {
		SetVoronoiSetting();
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 ) ) {
		SetZoomSetting();
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_PLUS ) ) {
		Map* currentMap = g_theGame->GetCurrentMap();
		int playerNum = currentMap->GetPlayerNum();
		if( playerNum <= 3 ) {
			currentMap->CreatePlayer();
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_MINUS ) ) {
		Map* currentMap = g_theGame->GetCurrentMap();
		int playerNum = currentMap->GetPlayerNum();
		if( playerNum > 1 ) {
			currentMap->DestroyPlayerWithIndex( playerNum - 1 );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1 ) ) {
		if( g_theCameraSystem->GetSplitScreenState() == VORONOI_SPLIT ) {
			g_theCameraSystem->SetAllowMerge( false );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_2 ) ) {
		if( g_theCameraSystem->GetSplitScreenState() == VORONOI_SPLIT ) {
			g_theCameraSystem->SetAllowMerge( true );
		}
	}
}

void Game::UpdateGame( float deltaSeconds )
{
	CheckIfExit();
	m_world->UpdateWorld( deltaSeconds );
	g_theCameraSystem->Update( deltaSeconds );
}

void Game::UpdateUI( )
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if( m_useSystemUI ) {
		UpdateSystemUI();
	}
	else {
		UpdateCameraWindowUI();
		UpdateCameraFrameUI();
		UpdateCameraShakeUI();
		UpdateMultiplayerStrategyUI();
		UpdateSystemDebugModeUI();
	}
}

void Game::UpdateSystemUI()
{
	static const char* cameraWindowStateItems[]{ "No Camera Window", "Camera Window" };
	static const char* cameraSnappingStateItems[]{ "No Camera Snapping", "Position Snapping", "Horizontal Lock", "Vertical Lock", "Position lock" };
	static const char* cameraShakeStateItems[]{ "Position shake", "Rotation shake", "Blend Shake" };
	static const char* cameraFrameStateItems[]{ "No Framing", "Forward Framing", "Projectile Framing", "Cue Framing", "Blend Framing" };
	static const char* splitScreenStateItems[]{ "No Split Screen", "Axis Aligned Split Screen", "Voronoi Split Screen" };
	static const char* noSplitScreenStrategyItems[]{ "No Strategy", "Zoom To Fit", "Kill And Teleport" };
	static const char* postVoronoiSettingItems[]{ "No Post Effect", "Push Plane And Rearrange Voronoi Vertex", "Rearrange Voronoi point", "Health based" };
	static const char* VoronoiAreaCheckingItems[]{ "Polygon Area", "Incircle Radius" };

	static uint cameraWindowStateIndex				= 0;
	static uint cameraSnappingStateIndex			= 0;
	static uint cameraShakeStateIndex				= 0;
	static uint cameraFrameStateIndex				= 0;
	static uint splitScreenStateIndex				= 0;
	static uint lastFrameSplitScreenStateIndex		= 0;
	static uint noSplitScreenStrategyIndex			= 0;
	static uint postVoronoiSettingIndex				= 0;

	static uint voronoiAreaCheckingIndex			= 0;

	static float dynamicAxisAlignedSplitScreenRatio = 0.5f;

	// camera window
	static Vec2 cameraWindowSize					= g_theCameraSystem->GetCameraWindowSize();

	// camera frame
	static float forwardCameraFrameDistance			= g_theCameraSystem->GetForwardVelocityCameraFrameDist();
	static float projectileCameraFrameDistance		= g_theCameraSystem->GetAimCameraFrameDist();

	// camera shake
	static float positionalShakeMaxDist				= g_theCameraSystem->GetPositionalShakeMaxDist();
	static float rotationalShakeMaxDeg				= g_theCameraSystem->GetRotationalShakeMaxDeg();

	// voronoi
	static int postVoronoiIterationNum				= g_theCameraSystem->GetPostVoronoiIterationNum();
	static bool doesVoronoiAllowMerge				= g_theCameraSystem->GetAllowMergeVoronoi();

	// split screen
	lastFrameSplitScreenStateIndex					= (uint)g_theCameraSystem->GetSplitScreenState();

	// debug flag 
	static bool doesShowCameraWindow				= false;
	static bool doesShowAnchorpoints				= false;
	static bool doesShowCameraPos					= false;

	ImGui::Begin( "2D Multiplayer Camera System" );
	ImGui::Combo( "Camera Window State", (int*)&cameraWindowStateIndex, cameraWindowStateItems, IM_ARRAYSIZE( cameraWindowStateItems ) );
	ImGui::InputFloat( "x", &cameraWindowSize.x );
	ImGui::InputFloat( "y", &cameraWindowSize.y );
	if( ImGui::Button( "confirm", ImVec2( 80.f, 20.f ) ) ) {
		g_theCameraSystem->SetCameraWindowSize( cameraWindowSize );
	}

	// debug flags
	ImGui::Text( "Debug settings" );
	ImGui::Checkbox( "Show Camera Position", &doesShowCameraPos );
	ImGui::Checkbox( "Show Camera Window", &doesShowCameraWindow );
	ImGui::Checkbox( "Show Voronoi Anchor points", &doesShowAnchorpoints );
	ImGui::Combo( "Camera Snapping State", (int*)&cameraSnappingStateIndex, cameraSnappingStateItems, IM_ARRAYSIZE( cameraSnappingStateItems ) );

	ImGui::Combo( "Camera Frame State", (int*)&cameraFrameStateIndex, cameraFrameStateItems, IM_ARRAYSIZE( cameraFrameStateItems ) );
	ImGui::InputFloat( "Forward frame distance", &forwardCameraFrameDistance );
	ImGui::InputFloat( "Projectile frame distance", &projectileCameraFrameDistance );


	ImGui::Combo( "Camera Shake State", (int*)&cameraShakeStateIndex, cameraShakeStateItems, IM_ARRAYSIZE( cameraShakeStateItems ) );
	ImGui::SliderFloat( "position shake max distance", &positionalShakeMaxDist, 0.f, 20.f );
	ImGui::SliderFloat( "rotation shake max degree", &rotationalShakeMaxDeg, 0.f, 20.f );

	ImGui::Combo( "Split Screen State", (int*)&splitScreenStateIndex, splitScreenStateItems, IM_ARRAYSIZE( splitScreenStateItems ) );
	if( splitScreenStateIndex == (uint)SplitScreenState::NO_SPLIT_SCREEN ) {
		ImGui::Combo( "No Split Screen Strategy", (int*)&noSplitScreenStrategyIndex, noSplitScreenStrategyItems, IM_ARRAYSIZE( noSplitScreenStrategyItems ) );
	}
	else if( splitScreenStateIndex == (uint)SplitScreenState::VORONOI_SPLIT ) {
		ImGui::Combo( "Post Voronoi Setting", (int*)&postVoronoiSettingIndex, postVoronoiSettingItems, IM_ARRAYSIZE( postVoronoiSettingItems ) );
		if( postVoronoiSettingIndex != (uint)PostVoronoiSetting::NO_POST_EFFECT ) {
			ImGui::Combo( "Voronoi Area Check State", (int*)&voronoiAreaCheckingIndex, VoronoiAreaCheckingItems, IM_ARRAYSIZE( VoronoiAreaCheckingItems ) );
			ImGui::SliderInt( "Post voronoi iteration num", &postVoronoiIterationNum, 1, 20 );
		}
		ImGui::Checkbox( "Allow merge", &doesVoronoiAllowMerge );
	}
	else if( splitScreenStateIndex == (uint)SplitScreenState::AXIS_ALIGNED_SPLIT ) {
		if( g_theCameraSystem->GetControllersNum() == 2 ) {
			ImGui::SliderFloat( "Screen Ratio", &dynamicAxisAlignedSplitScreenRatio, 0.f, 1.f );
			if( ImGui::Button( "Start", ImVec2( 40.f, 20.f ) ) ) {
				float totalTime = 5.f;
				float currentMultipleFactor = g_theCameraSystem->GetCameraControllerWithIndex( 0 )->GetCurrentMultipleFactor();
				float smoothTime = 0.f;
				if( dynamicAxisAlignedSplitScreenRatio == currentMultipleFactor ) {
					smoothTime = 1.f;
				}
				else {
					float deltaRatio = abs( dynamicAxisAlignedSplitScreenRatio - currentMultipleFactor );
					smoothTime = RangeMapFloat( 0.f, 1.f, 1.f, totalTime, deltaRatio );
				}
				g_theCameraSystem->SetDynamicAxisAlignedSplitScreenMultipleFactors( smoothTime, dynamicAxisAlignedSplitScreenRatio, 1.f - dynamicAxisAlignedSplitScreenRatio );
			}
		}
	}

	ImGui::End();

	// frame settings
	g_theCameraSystem->SetForwardVelocityFrameDistance( forwardCameraFrameDistance );
	g_theCameraSystem->SetAimFocusDistance( projectileCameraFrameDistance );

	// camera window and snap
	g_theCameraSystem->SetCameraWindowState( (CameraWindowState)cameraWindowStateIndex );
	g_theCameraSystem->SetCameraSnappingState( (CameraSnappingState)cameraSnappingStateIndex );

	// camera shake
	g_theCameraSystem->SetCameraShakeState( (CameraShakeState)cameraShakeStateIndex );
	g_theCameraSystem->SetPositionalShakeMaxDist( positionalShakeMaxDist );
	g_theCameraSystem->SetRotationalShakeMaxDeg( rotationalShakeMaxDeg );

	// split screen
	if( lastFrameSplitScreenStateIndex != splitScreenStateIndex ) {
		g_theCameraSystem->SetSplitScreenState( (SplitScreenState)splitScreenStateIndex );
	}
	g_theCameraSystem->SetNoSplitScreenStrat( (NoSplitScreenStrat)noSplitScreenStrategyIndex );
	g_theCameraSystem->SetPostVoronoiSetting( (PostVoronoiSetting)postVoronoiSettingIndex );
	g_theCameraSystem->SetVoronoiAreaCheckState( (VoronoiAreaCheckState)voronoiAreaCheckingIndex );
	g_theCameraSystem->SetPostVoronoiIterationNum( postVoronoiIterationNum );
	g_theCameraSystem->SetAllowMerge( doesVoronoiAllowMerge );

	// debug flags
	g_theCameraSystem->SetDoesDebugCameraWindow( doesShowCameraWindow );
	g_theCameraSystem->SetDoesDebugAnchorPoints( doesShowAnchorpoints );
	g_theCameraSystem->SetDoesDebugCameraPosition( doesShowCameraPos );
}

void Game::UpdateCameraWindowUI()
{
	CameraWindowState windowState		= g_theCameraSystem->GetCameraWindowState();
	CameraSnappingState snappingState	= g_theCameraSystem->GetCameraSnappingState();
	static const char* cameraSnappingStateItems[]{ "No Camera Snapping", "Position Snapping", "Horizontal Position Snapping", "Vertical Position Snapping" };
	static uint cameraSnappingStateIndex = 0;
	static bool doesUseCameraWindow;
	static bool doesUseCameraSnapping;
	static Vec2 windowSize = g_theCameraSystem->GetCameraWindowSize();
	doesUseCameraWindow = ( windowState == USE_CAMERA_WINDOW );
	doesUseCameraSnapping = ( snappingState == POSITION_SNAPPING );
	ImGui::Begin( "Camera Window" );
	ImGui::Checkbox( "Use Camera Window", &doesUseCameraWindow );
	ImVec2 buttonSize = ImVec2( 120.f, 20.f );
	if( doesUseCameraWindow ) {
		ImGui::SliderFloat( "Width", &windowSize.x, MIN_CAMERA_WINDOW_WIDTH, MAX_CAMERA_WINDOW_WIDTH, "%.2f", 1.f );
		ImGui::SliderFloat( "Height", &windowSize.y, MIN_CAMERA_WINDOW_HEIGHT, MAX_CAMERA_WINDOW_HEIGHT, "%.2f", 1.f );
		if( ImGui::Button( "Vertical Lock", buttonSize ) ) {
			windowSize.y = MIN_CAMERA_WINDOW_HEIGHT;
			ImGui::SameLine();
		}
		else if( ImGui::Button( "Horizontal Lock", buttonSize ) ) {
			windowSize.x = MIN_CAMERA_WINDOW_WIDTH;
			ImGui::SameLine();
		}
		else if( ImGui::Button( "Position Lock", buttonSize ) ) {
			windowSize.x = MIN_CAMERA_WINDOW_WIDTH;
			windowSize.y = MIN_CAMERA_WINDOW_HEIGHT;
			ImGui::SameLine();
		}
		else if( ImGui::Button( "Reset", buttonSize ) ) {
			windowSize.x = DEFAULT_CAMERA_WINDOW_WIDTH;
			windowSize.y = DEFAULT_CAMERA_WINDOW_HEIGHT;
		}
		ImGui::Combo( "Camera snapping states", (int*)&cameraSnappingStateIndex, cameraSnappingStateItems, IM_ARRAYSIZE( cameraSnappingStateItems ) );
	}
	ImGui::End();


	if( doesUseCameraWindow ) {
		g_theCameraSystem->SetCameraWindowState( USE_CAMERA_WINDOW );
		g_theCameraSystem->SetCameraWindowSize( windowSize );
	}
	else {
		g_theCameraSystem->SetCameraWindowState( NO_CAMERA_WINDOW );
	}
	g_theCameraSystem->SetCameraSnappingState( (CameraSnappingState)cameraSnappingStateIndex );
}

void Game::UpdateCameraFrameUI()
{
	// camera frame
	static bool	 enableCameraFrame				= false;
	static float fwdVelCameraFrameDist			= g_theCameraSystem->GetForwardVelocityCameraFrameDist();
	static float aimCameraFrameDist				= g_theCameraSystem->GetAimCameraFrameDist();
	static float fwdVelCameraFrameRatio			= g_theCameraSystem->GetControllerForwardVelocityCameraFrameRatio( 0 );
	static float aimCameraFrameRatio			= g_theCameraSystem->GetControllerAimCameraFrameRatio( 0 );
	static float cueCameraFrameRatio			= g_theCameraSystem->GetControllerCueCameraFrameRatio( 0 );

	ImGui::Begin( "Camera Frame" );
	ImGui::Checkbox( "Enable Camera Frame", &enableCameraFrame );
	ImGui::SliderFloat( "Forward velocity frame dist", &fwdVelCameraFrameDist, 0.f, MAX_FORWARD_VELOCITY_FRAME_DIST, "%.2f", 1.f );
	ImGui::SliderFloat( "Aim frame dist", &aimCameraFrameDist, 0.f, MAX_FORWARD_VELOCITY_FRAME_DIST, "%.2f", 1.f );
	ImGui::SliderFloat( "Forward velocity frame ratio", &fwdVelCameraFrameRatio, 0.f, 1.f, "%.2f", 1.f );
	ImGui::SliderFloat( "Aim frame ratio", &aimCameraFrameRatio, 0.f, 1.f, "%.2f", 1.f );
	ImGui::SliderFloat( "Cue frame ratio", &cueCameraFrameRatio, 0.f, 1.f, "%.2f", 1.f );
	ImGui::End();
	
	g_theCameraSystem->SetForwardVelocityFrameDistance( fwdVelCameraFrameDist );
	g_theCameraSystem->SetAimFocusDistance( aimCameraFrameDist );
	g_theCameraSystem->SetControllerForwardVelocityFrameRatio( 0, fwdVelCameraFrameRatio );
	g_theCameraSystem->SetControllerAimFocusRatio( 0, aimCameraFrameRatio );
	g_theCameraSystem->SetControllerCueFocusRatio( 0, cueCameraFrameRatio );
}

void Game::UpdateCameraShakeUI()
{
	static float positionalShakeMaxDist				= DEFAULT_MAX_POSITION_SHAKE_DISTANCE;
	static float rotationalShakeMaxDeg				= DEFAULT_MAX_ROTATION_SHAKE_DEGREE;
	static float truama								= 0.f;
	CameraController* demoController = g_theCameraSystem->GetCameraControllerWithIndex( 0 );
	if( demoController ) {
		truama = demoController->GetTrauma();
	}
	ImGui::Begin( "Camera Shake" );
	ImGui::SliderFloat( "Trauma", &truama, 0.f, 1.f, "%.2f", ImGuiSliderFlags_NoInput );
	if( ImGui::Button( "Add trauma", ImVec2( 80.f, 20.f ) ) ) {
		g_theCameraSystem->AddCameraShake( 0, 0.3f );
	}
	ImGui::SliderFloat( "Positional shake max Distances", &positionalShakeMaxDist, 0.f, DEFAULT_MAX_POSITION_SHAKE_DISTANCE, "%.2f", 1.f );
	ImGui::SliderFloat( "Rotational shake max Degrees", &rotationalShakeMaxDeg, 0.f, DEFAULT_MAX_ROTATION_SHAKE_DEGREE, "%.2f", 1.f );
	ImGui::End();

	g_theCameraSystem->SetPositionalShakeMaxDist( positionalShakeMaxDist );
	g_theCameraSystem->SetRotationalShakeMaxDeg( rotationalShakeMaxDeg );
}

void Game::UpdateMultiplayerStrategyUI()
{
	static const char* multiplayerItems[]{ "No Strategy", "Zoom To Fit", "Kill And Teleport", "Axis Aligned Split Screen", "Voronoi Split Screen" };
	static const char* VoronoiBalanceCriterionItems[]{ "Polygon Area", "Incircle Radius" };
	static int multiplayerItemIndex			= 0;
	static int balanceCriterionIndex		= 0;
	static int voronoiBalanceIterationNum	= 0;
	static bool isVoronoiBalanced			= false;
	static bool doesAllowMerge				= false;

	ImGui::Begin( "Multiplayer Strategy" );
	ImGui::Combo( "Mutiplayer strategy", &multiplayerItemIndex, multiplayerItems, IM_ARRAYSIZE( multiplayerItems )  );
	if( multiplayerItemIndex < 3 ) {
		g_theCameraSystem->SetSplitScreenState( NO_SPLIT_SCREEN );
		if( multiplayerItemIndex == 1 ) {
			g_theCameraSystem->SetNoSplitScreenStrat( ZOOM_TO_FIT );
		}
		else if( multiplayerItemIndex == 2 ) {
			g_theCameraSystem->SetNoSplitScreenStrat( KILL_AND_TELEPORT );
		}
	}
	else {
		if( multiplayerItemIndex == 3 ) {
			g_theCameraSystem->SetSplitScreenState( AXIS_ALIGNED_SPLIT );
		}
		else if ( multiplayerItemIndex == 4 ) {
			g_theCameraSystem->SetSplitScreenState( VORONOI_SPLIT );
			if( g_theCameraSystem->GetControllersNum() > 2 ) {
				g_theCameraSystem->SetAllowMerge( false );
				ImGui::Checkbox( "Balance the cell", & isVoronoiBalanced  );
				if( isVoronoiBalanced ) {
					g_theCameraSystem->SetPostVoronoiSetting( BALANCED_VORONOI_DIAGRAM );
					ImGui::Combo( "Balance criterion", &balanceCriterionIndex, VoronoiBalanceCriterionItems, IM_ARRAYSIZE( VoronoiBalanceCriterionItems ));
					ImGui::SliderInt( "Balance Iteration number", &voronoiBalanceIterationNum, 0, 20 );
					g_theCameraSystem->SetPostVoronoiIterationNum( voronoiBalanceIterationNum );
					if( balanceCriterionIndex == 0 ) {
						g_theCameraSystem->SetVoronoiAreaCheckState( POLYGON_AREA );
					}
					else if( balanceCriterionIndex == 1 ) {
						g_theCameraSystem->SetVoronoiAreaCheckState( INCIRCLE_RADIUS );
					}
				}
			}
			else {
				ImGui::Checkbox( "Allow merge", &doesAllowMerge );
				g_theCameraSystem->SetAllowMerge( doesAllowMerge );
			}
		}
	}
	ImGui::End();
}

void Game::UpdateSystemDebugModeUI()
{
	// debug flag 
	static bool isDebug								= false;
	static bool doesShowCameraWindow				= false;
	static bool doesShowAnchorpoints				= false;
	static bool doesShowCameraPos					= false;
	ImGui::Begin( "Camera Debug Mode" );
	ImGui::Checkbox( "Debug Mode", &isDebug );
	if( isDebug ) {
		ImGui::Checkbox( "Show Camera Window", &doesShowCameraWindow );
		ImGui::Checkbox( "Show Voronoi Points", &doesShowAnchorpoints );
		ImGui::Checkbox( "Show Camera Position", &doesShowCameraPos );
	}
	ImGui::End();
	g_theCameraSystem->SetIsDebug( isDebug );
	g_theCameraSystem->SetDoesDebugCameraWindow( doesShowCameraWindow );
	g_theCameraSystem->SetDoesDebugAnchorPoints( doesShowAnchorpoints );
	g_theCameraSystem->SetDoesDebugCameraPosition( doesShowCameraPos );

}

void Game::RenderGameUI() const
{

}

void Game::PrepareDemo1()
{
	//
}

void Game::InitializeCameraSystemSetting( int mapIndex )
{
	if( mapIndex == 1 ) {
		m_useUI = false;
		g_theCameraSystem->SetSplitScreenState( NO_SPLIT_SCREEN );
		g_theCameraSystem->SetNoSplitScreenStrat( COMBINATION_ZOOM_AND_KILL );
	}
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_X ) ) {
		g_theApp->HandleQuitRequested();
	}
}



//////////////////////////////////////////////////////////////////////////


// loading
//////////////////////////////////////////////////////////////////////////
void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "squirrelFont", "Data/Fonts/SquirrelFixedFont" );	
	m_playerTexture				= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankBase.png" );
	m_playerBarrelTexture		= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/PlayerTankTop.png" );
	m_enemyTexture				= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyTank1.png" );
	m_enemyBarrelTexture		= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyCannon.png" );
	m_playerProjectileTexture	= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/FriendlyBullet.png" );
	m_enemyProjectileTexture	= g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/EnemyBullet.png" );
}

void Game::LoadDefs()
{
	MapDefinition::PopulateDefinitionsFromXmlFile( MAP_DEF_FILE_PATH );
	TileDefinition::PopulateDefinitionFromXmlFile( TILE_DEF_FILE_PATH );
	//ActorDefinition::PopulateDefinitionFromXmlFile( ACTOR_DEF_FILE_PATH );
}

void Game::SetVoronoiSetting()
{
	g_theCameraSystem->SetSplitScreenState( VORONOI_SPLIT );
	g_theCameraSystem->SetPostVoronoiSetting( BALANCED_VORONOI_DIAGRAM );
	g_theCameraSystem->SetVoronoiAreaCheckState( INCIRCLE_RADIUS );
	g_theCameraSystem->SetDoesUseCameraFrame( true );
	for( int i = 0; i < g_theCameraSystem->GetControllersNum(); i++ ) {
		g_theCameraSystem->SetControllerUseCameraFrame( i, true );
		g_theCameraSystem->SetControllerAimFocusRatio( i, 1.f );
		g_theCameraSystem->SetAimFocusDistance( 2.5f );
	}
	g_theCameraSystem->SetPostVoronoiIterationNum( 5 );
	g_theCameraSystem->SetAllowMerge( false );
	g_theCameraSystem->SetAllowBlend( false );
}

void Game::SetZoomSetting()
{
	g_theCameraSystem->SetSplitScreenState( NO_SPLIT_SCREEN );
	g_theCameraSystem->SetNoSplitScreenStrat( COMBINATION_ZOOM_AND_KILL );
}

//////////////////////////////////////////////////////////////////////////

bool SetCameraAsymptoticValue( EventArgs& args )
{
	float asymptoticValue = args.GetValue( std::to_string( 0 ), 0.f );
	std::vector<CameraController*> controllers = g_theCameraSystem->GetCameraControllers();
	for( int i = 0; i < controllers.size(); i++ ) {
		controllers[i]->SetAsymptoticValue( asymptoticValue );
	}
	return true;
}

bool SpawnItem( EventArgs& args )
{
	Vec2 pos = args.GetValue( std::to_string( 0 ), Vec2::ZERO );
	Map* currentMap = g_theGame->m_world->GetCurrentMap();
	currentMap->SpawnNewItem( pos );
	return true;
}

bool DeletePlayer( EventArgs& args )
{
	std::string playerIndex = args.GetValue( std::to_string( 0 ), "0" );
	
	Map* currentMap = g_theGame->GetCurrentMap();
	currentMap->DestroyPlayerWithIndex( atoi(playerIndex.c_str()) );
	return true;
}

bool SetIsSmooth( EventArgs& args )
{
	std::string isSmoothText = args.GetValue( std::to_string( 0 ), std::string() );
	bool isSmooth = GetBoolFromText( isSmoothText.c_str() );
	g_theCameraSystem->SetControllerSmooth( isSmooth );
	return true;
}

bool LoadMap( EventArgs& args )
{
	std::string mapIndex = args.GetValue( std::to_string( 0 ), std::string() );
	int iMapIndex =atoi( mapIndex.c_str() );
	if( g_theGame->m_world->GetCurrentMapIndex() != iMapIndex ){
		Map* currentMap = g_theGame->m_world->GetCurrentMap();
		currentMap->DestroyAllPlayers();
		g_theGame->InitializeCameraSystemSetting( iMapIndex );
		g_theGame->m_world->SetCurrentMapIndex( atoi( mapIndex.c_str() ));
	}
	return true;
}
