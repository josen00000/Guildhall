#include "game.hpp"
#include "Game/App.hpp"
#include "Game/Camera/CameraSystem.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/World.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"

//////////////////////////////////////////////////////////////////////////
// global variable
//////////////////////////////////////////////////////////////////////////
// Game
//////////////////////////////////////////////////////////////////////////
extern App*				g_theApp;
extern CameraSystem*	g_theCameraSystem;
extern Game*			g_theGame;

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
	m_world = World::CreateWorld( 1 );
	g_theCameraSystem = new CameraSystem();
	g_theCameraSystem->Startup();

	g_theConsole->AddCommandToCommandList( std::string( "set_asymptotic"), std::string( "Asymptotic value for camera controller" ), SetCameraAsymptoticValue );
	g_theConsole->AddCommandToCommandList( std::string( "spawn_item"), std::string( "spawn new item" ), SpawnItem );
	g_theConsole->AddCommandToCommandList( std::string( "delete_player"), std::string( "delete player with index" ), DeletePlayer );
	g_theConsole->AddCommandToCommandList( std::string( "set_camera_smooth" ), std::string( "set is controller smooth motion" ), SetIsSmooth );
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
	UpdateGame( deltaSeconds );
	UpdateUI( deltaSeconds );

}

void Game::RenderGame() const
{
	m_world->RenderWorld();
	g_theCameraSystem->DebugRender();
}

void Game::RenderUI() const
{

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
		CameraWindowState currentState = g_theCameraSystem->GetCameraWindowState();
		if( currentState < CameraWindowState::NUM_OF_CAMERA_WINDOW_STATE ) {
			currentState = static_cast<CameraWindowState>( currentState + 1 );
		}
		if( currentState == CameraWindowState::NUM_OF_CAMERA_WINDOW_STATE ) {
			currentState = static_cast<CameraWindowState>( (uint) 0 );
		}
		g_theCameraSystem->SetCameraWindowState( currentState );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F3 ) ) {
		CameraSnappingState currentState = g_theCameraSystem->GetCameraSnappingState();
		if( currentState < CameraSnappingState::NUM_OF_CAMERA_SNAPPING_STATE ) {
			currentState = static_cast<CameraSnappingState>( currentState + 1 );
		}
		if( currentState == CameraSnappingState::NUM_OF_CAMERA_SNAPPING_STATE ) {
			currentState = static_cast<CameraSnappingState>((uint)0);
		}
		g_theCameraSystem->SetCameraSnappingState( currentState );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F4 ) ) {
		CameraShakeState currentState = g_theCameraSystem->GetCameraShakeState();
		if( currentState < CameraShakeState::NUM_OF_SHAKE_STATE - 1 ) {
			currentState = static_cast<CameraShakeState>( currentState + 1 );
		}
		else {
			currentState = static_cast<CameraShakeState>( (uint)0 );
		}
		g_theCameraSystem->SetCameraShakeState( currentState );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F5 ) ) {
		CameraFrameState currentState = g_theCameraSystem->GetCameraFrameState();
		if( currentState < CameraFrameState::NUM_OF_FRAME_STATE - 1 ) {
			currentState = static_cast<CameraFrameState>(currentState + 1);
		}
		else {
			currentState = static_cast<CameraFrameState>((uint)0);
		}
		g_theCameraSystem->SetCameraFrameState( currentState );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F6 ) ) {
		SplitScreenState currentState = g_theCameraSystem->GetSplitScreenState();
		if( currentState < SplitScreenState::NUM_OF_SPLIT_SCREEN_STATE - 1 ) {
			currentState = static_cast<SplitScreenState>(currentState + 1);
		}
		else {
			currentState = static_cast<SplitScreenState>((uint)0);
		}
		g_theCameraSystem->SetSplitScreenState( currentState );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F7 ) ) {
		NoSplitScreenStrat currentStrat = g_theCameraSystem->GetNoSplitScreenStrat();
		if( currentStrat < NoSplitScreenStrat::NUM_OF_NO_SPLIT_SCREEN_STRAT - 1 ) {
			currentStrat = static_cast<NoSplitScreenStrat>(currentStrat + 1);
		}
		else {
			currentStrat = static_cast<NoSplitScreenStrat>((uint)0);
		}
		g_theCameraSystem->SetNoSplitScreenStrat( currentStrat );
	}

	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_SHIFT ) ) {
		Map* currentMap = m_world->GetCurrentMap();
		if( currentMap != nullptr ) {
			currentMap->ShiftPlayer();
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ) {
		Map* currentMap = m_world->GetCurrentMap();
		if( currentMap != nullptr ) {
			currentMap->CreatePlayer();
		 }
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_I ) ) {
		Map* currentMap = m_world->GetCurrentMap();
		if( currentMap != nullptr ) {
			currentMap->SpawnNewItem( Vec2( 13, 10 ) );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_E ) ) {
		Map* currentMap = m_world->GetCurrentMap();
		if( currentMap != nullptr ) {
			Vec2 spawnPos = (Vec2)currentMap->GetRandomInsideNotSolidTileCoords();
			currentMap->SpawnNewEnemy( spawnPos );
		}
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_1 ) ) {
		g_theCameraSystem->AddCameraShake( 0, 0.5f );
	}
}

void Game::UpdateGame( float deltaSeconds )
{
	// check exit
	// update world
	CheckIfExit();
	m_world->UpdateWorld( deltaSeconds );
	g_theCameraSystem->Update( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{

}

void Game::RenderGameUI() const
{

}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ) {
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
