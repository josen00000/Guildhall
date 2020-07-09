#include <windows.h>
#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameObject.hpp"
#include "Game/Player.hpp"
#include "Game/UIButton.hpp"
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
	CreateAttractButtons();
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
	CheckIfExit();
	switch( m_gameState )
	{
	case ATTRACT_SCREEN:
		UpdateAttractScreen();
		break;
	case IN_GAME:
		CleanDestroyedObjects();
		m_world->UpdateWorld( deltaSeconds );
		break;
	case GAME_END:
		break;
	default:
		break;
	}

}

void Game::UpdateAttractScreen()
{
	HandleAttractInput();
	for( int i = 0; i < m_buttons.size(); i++ ) {
		m_buttons[i]->UpdateButton();
	}
}

void Game::RenderAttractScreen() const 
{
	for( int i = 0; i < m_buttons.size(); i++ ) {
		m_buttons[i]->RenderButton();
	}
}

void Game::UpdateUI( float deltaSeconds )
{
	Map* currentMap = m_world->GetCurrentMap();
	Player* player = currentMap->GetPlayer();
	m_playerHP = player->GetHealth();
	m_playerAttackStrength = player->GetAttackStrength();
	UNUSED( deltaSeconds );
}


void Game::RenderGameUI() const
{
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, -2.f ), Vec2::ZERO, Rgba8::RED, "HP: %i", (int)m_playerHP );
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, -4.f ), Vec2::ZERO, Rgba8::RED, "Attack Strength: %i", (int)m_playerAttackStrength );
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


void Game::HandleAttractInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		SelectPreviousButton();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		SelectNextButton();
	}
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ENTER ) ) {
		m_buttons[m_currentButtonIndex]->ExecuteTargetFunction();
	}
}

void Game::RenderGame() const
{
	switch( m_gameState )
	{
	case ATTRACT_SCREEN:
		//RenderAttractScreen();
		break;
	case IN_GAME:
		m_world->RenderWorld();
		break;
	case GAME_END:
		break;
	default:
		break;
	}
}

void Game::RenderUI() const
{
	switch( m_gameState )
	{
	case ATTRACT_SCREEN:
		RenderAttractScreen();
		break;
	case IN_GAME:
		RenderGameUI();
		break;
	case GAME_END:
		break;
	default:
		break;
	}
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

void Game::SelectPreviousButton()
{
	if( m_currentButtonIndex != 0 ) {
		m_buttons[m_currentButtonIndex]->SetIsSelecting( false );
		m_currentButtonIndex--;
		m_buttons[m_currentButtonIndex]->SetIsSelecting( true );
	}
}

void Game::SelectNextButton()
{
	if( m_currentButtonIndex < m_buttons.size() - 1 ) {
		m_buttons[m_currentButtonIndex]->SetIsSelecting( false );
		m_currentButtonIndex++;
		m_buttons[m_currentButtonIndex]->SetIsSelecting( true );
	}
}

void Game::CreateAttractButtons()
{
	float buttonWidth = 50.f;
	float buttonHeight = 20.f;
	Vec2 centerPos = Vec2( 100.f, 80.f );
	UIButton* startButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos, Vec2( buttonWidth, buttonHeight ), "START" );
	UIButton* settingButton = UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -25.f ), Vec2( buttonWidth, buttonHeight ), "SETTING" );
	UIButton* quitButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -50.f ), Vec2( buttonWidth, buttonHeight ), "QUIT" );


	startButton->SetEventName( "StartGame" );
	settingButton->SetEventName( "LoadSetting" );
	quitButton->SetEventName( "QuitGame" );

	g_theEventSystem->SubscribeMethodToEvent( "StartGame", this, &Game::StartGame );
	g_theEventSystem->SubscribeMethodToEvent( "QuitGame", this, &Game::QuitGame );

	startButton->SetIsSelecting( true );
	m_buttons.push_back( startButton );
	m_buttons.push_back( settingButton );
	m_buttons.push_back( quitButton );
}

bool Game::StartGame( EventArgs& args )
{
	m_gameState = IN_GAME;
	return true;
}

bool Game::QuitGame( EventArgs& args )
{
	m_isAppQuit = true;
	return true;
}

