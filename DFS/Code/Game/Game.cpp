#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameObject.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/World.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/PolygonCollider2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Platform/Window.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;	
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;
extern DevConsole*		g_theConsole;
extern Physics2D*		g_thePhysics;
extern Window*			g_theWindow;

Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_isAppQuit		= false;
	LoadAssets();

	m_world = World::CreateWorld( 1 );
}

void Game::RestartGame()
{
	m_world->CreateMaps();
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
}

void Game::RunFrame( float deltaSeconds )
{
	UpdateGame( deltaSeconds );
	HandleKeyboardInput();
}

void Game::UpdateGame( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	CheckIfExit();
	m_gameCamera->SetClearMode( CLEAR_COLOR_BIT, Rgba8::RED, 0.0f, 0 );
	CleanDestroyedObjects();
	m_world->UpdateWorld( deltaSeconds );
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}


void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
		g_theApp->HandleQuitRequested();
	}
}


void Game::HandleKeyboardInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F5 ) ) {
		RestartGame();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F1 ) ) {
		m_isDebug = !m_isDebug;
	}
}


void Game::RenderGame() const
{
	m_world->RenderWorld();
}

void Game::RenderUI() const
{
	if( m_isDebug ) {
		int index = 0;
		DebugAddScreenText( Vec4( 0, 0.5, 0, -2 * index ), Vec2::ZERO, 5.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, "debug mode is " + GetStringFromBool( m_isDebug ) );
	}
}


void Game::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
}

void Game::DeleteGameObject( GameObject* obj )
{
	if( obj == nullptr ) { return; }

	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == obj ) {
			delete m_gameObjects[objIndex];
			m_gameObjects[objIndex] = nullptr;
		}
	}
}

void Game::CleanDestroyedObjects()
{

}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	LoadDefs();
}

void Game::LoadDefs()
{
	// load map def
	XmlDocument mapDefFile;
	mapDefFile.LoadFile( MAP_DEF_FILE_PATH );
	XmlElement* mapRootELement = mapDefFile.RootElement();
	XmlElement* mapDefElement = mapRootELement->FirstChildElement();
	while( mapDefElement ) {
		MapDefinition::PopulateDefinitionFromXmlElement( *mapDefElement );
		mapDefElement = mapDefElement->NextSiblingElement();
	}

	// load tile def
	TileDefinition::PopulateDefinitionFromXmlFile( TILE_DEF_FILE_PATH );

	// load actor def
	ActorDefinition::PopulateDefinitionFromXmlFile( ACTOR_DEF_FILE_PATH );
}

