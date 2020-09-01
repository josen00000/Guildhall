#include "game.hpp"
#include "Game/App.hpp"
#include "Game/ActorDefinition.hpp"
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
		m_isDebug = !m_isDebug;
	}
}

void Game::UpdateGame( float deltaSeconds )
{
	// check exit
	// update world
	CheckIfExit();
	m_world->UpdateWorld( deltaSeconds );
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

void Game::CheckIfDebug()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_isDebug = true;
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
	ActorDefinition::PopulateDefinitionFromXmlFile( ACTOR_DEF_FILE_PATH );
}
//////////////////////////////////////////////////////////////////////////
