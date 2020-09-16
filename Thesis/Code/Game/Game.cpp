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

// Engine
//////////////////////////////////////////////////////////////////////////
extern InputSystem*		g_theInputSystem;
extern BitmapFont*		g_squirrelFont;
extern RenderContext*	g_theRenderer;

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
	HandleInput();
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
//////////////////////////////////////////////////////////////////////////


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
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) ) {
		Map* currentMap = m_world->GetCurrentMap();
		if( currentMap != nullptr ) {
			currentMap->CreatePlayer();
		 }
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
}

void Game::LoadDefs()
{
	MapDefinition::PopulateDefinitionsFromXmlFile( MAP_DEF_FILE_PATH );
	TileDefinition::PopulateDefinitionFromXmlFile( TILE_DEF_FILE_PATH );
	//ActorDefinition::PopulateDefinitionFromXmlFile( ACTOR_DEF_FILE_PATH );
}
//////////////////////////////////////////////////////////////////////////
