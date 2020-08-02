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
extern AudioSystem*		g_theAudioSystem;

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
	//m_world = World::CreateWorld( 1 );
	g_theEventSystem->SubscribeMethodToEvent( "DebugMessage", this, &Game::DebugGameInfo );
}

void Game::StartGame()
{
	if( m_world == nullptr ) {
		delete m_world;
	}
	m_world = World::CreateWorld( 1 );
	//m_world->CreateMaps();
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
}

void Game::RunFrame( float deltaSeconds )
{
	UpdateGame( deltaSeconds );
	UpdateUI( deltaSeconds );
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
		if( m_showPauseMenu ) {
			UpdatePauseButtons();
		}
		break;
	case GAME_END:
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ENTER ) ) {
			StartGame();
			m_gameState = ATTRACT_SCREEN;
		}
		break;

	}

}

void Game::UpdateAttractScreen()
{
	HandleAttractInput();
	if( m_isOnSettingPage ) {
		for( int i = 0; i < m_settingButtons.size(); i++ ) {
			m_settingButtons[i]->UpdateButton();
		}
	}
	else {
		for( int i = 0; i < m_buttons.size(); i++ ) {
			m_buttons[i]->UpdateButton();
		}
	}
}

void Game::RenderAttractScreen() const 
{
	if( m_isOnSettingPage ) {
		for( int i = 0; i < m_settingButtons.size(); i++ ) {
			m_settingButtons[i]->RenderButton();
		}
	}
	else {
		for( int i = 0; i < m_buttons.size(); i++ ) {
			m_buttons[i]->RenderButton();
		}
	}
}

void Game::UpdateUI( float deltaSeconds )
{
	if( !m_world ){ return; }

	Map* currentMap = m_world->GetCurrentMap();
	Player* player = currentMap->GetPlayer();
	m_playerHP = player->GetHealth();
	m_playerAttackStrength = player->GetAttackStrength();
	m_enemyType = "";
	if( currentMap->GetIsFighting() ) {
		const Actor* enemy = currentMap->GetFightEnemy();
		m_enemyHP = enemy->GetHealth();
		m_enemyAttackStrength = enemy->GetAttackStrength();
		m_enemyType = enemy->GetType();
	}
	UNUSED( deltaSeconds );
}


void Game::RenderGameUI() const
{
	int index = 1;
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, index * -2.f ), Vec2::ZERO, Rgba8::RED, "Player HP: %i", (int)m_playerHP );
	index++;
	DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, index * -2.f ), Vec2::ZERO, Rgba8::RED, "Player Attack Strength: %i", (int)m_playerAttackStrength );
	index++;
	if( m_enemyType.compare( "" ) != 0 ) {
		DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, index * -2.f ), Vec2::ZERO, Rgba8::RED, "Enemy: %s", m_enemyType.c_str() );
		index++;
		DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, index * -2.f ), Vec2::ZERO, Rgba8::RED, "Enemy HP: %i", (int)m_enemyHP );
		index++;
		DebugAddScreenTextf( Vec4( 0.f, 1.f, 0.f, index * -2.f ), Vec2::ZERO, Rgba8::RED, "Enemy Attack Strength: %i", (int)m_enemyAttackStrength );
	}
	
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
		//g_theApp->HandleQuitRequested();
		if( m_gameState == IN_GAME ) {
			m_showPauseMenu = true;
			g_theApp->PauseGame();
		}
	}
}


void Game::HandleKeyboardInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_F5 ) ) {
		StartGame();
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
		if( m_isOnSettingPage ) {
			m_settingButtons[m_currentSettingButtonIndex]->ExecuteTargetFunction();
		}
		else {
			m_buttons[m_currentButtonIndex]->ExecuteTargetFunction();
		}
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
		if( m_showPauseMenu ) {
			for( int i = 0; i < m_pauseButtons.size(); i++ ) {
				m_pauseButtons[i]->RenderButton();
			}
		}
		break;
	case GAME_END:
		break;
	default:
		break;
	}
}

void Game::RenderUI() const
{
	std::string resultText = "";
	if( m_playerHP > 0 ) {
		resultText = "YOU WIN!";
	}
	else {
		resultText = "YOU LOSE!";
	}

	switch( m_gameState )
	{
	case ATTRACT_SCREEN:
		RenderAttractScreen();
		break;
	case IN_GAME:
		RenderGameUI();
		if( m_showPauseMenu ) {
			RenderPauseButtons();
		}
		break;
	case GAME_END:
		DebugAddScreenText( Vec4( 0.3f, 0.5f, 0.f, 0.f ), Vec2( 0.5f ), 10.f, Rgba8::RED, Rgba8::RED, 0.f, resultText );
		break;
	default:
		break;
	}
	if( m_isDebug ) {
		int index = 0;
		DebugAddScreenText( Vec4( 0, 0.5, 0, -2 * index ), Vec2::ZERO, 5.f, Rgba8::WHITE, Rgba8::WHITE, 0.f, "debug mode is " + GetStringFromBool( m_isDebug ) );
	}
}


void Game::SetGameEnd()
{
	m_gameState = GAME_END;
	g_theRenderer->ClearState();
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
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/SwordHit.mp3" );
	g_theAudioSystem->CreateOrGetSound( "Data/Audio/WalkFoot.mp3" );
	SoundID gameBGM = g_theAudioSystem->CreateOrGetSound( "Data/Audio/GameplayMusic.mp3" );
	m_gameBGMID = g_theAudioSystem->PlaySound( gameBGM, true, m_volume * m_backGroundVolumeScale );
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
	if( m_isOnSettingPage ) {
		if( m_currentSettingButtonIndex != 0 ) {
			m_settingButtons[m_currentSettingButtonIndex]->SetIsSelecting( false );
			m_currentSettingButtonIndex--;
			m_settingButtons[m_currentSettingButtonIndex]->SetIsSelecting( true );
		}
	}
	else {
		if( m_currentButtonIndex != 0 ) {
			m_buttons[m_currentButtonIndex]->SetIsSelecting( false );
			m_currentButtonIndex--;
			m_buttons[m_currentButtonIndex]->SetIsSelecting( true );
		}
	}
}

void Game::SelectNextButton()
{
	if( m_isOnSettingPage ) {
		if( m_currentSettingButtonIndex < m_settingButtons.size() - 1 ) {
			m_settingButtons[m_currentSettingButtonIndex]->SetIsSelecting( false );
			m_currentSettingButtonIndex++;
			m_settingButtons[m_currentSettingButtonIndex]->SetIsSelecting( true );
		}
	}
	else {
		if( m_currentButtonIndex < m_buttons.size() - 1 ) {
			m_buttons[m_currentButtonIndex]->SetIsSelecting( false );
			m_currentButtonIndex++;
			m_buttons[m_currentButtonIndex]->SetIsSelecting( true );
		}
	}
}

void Game::CreateAttractButtons()
{
	float buttonWidth = 90.f;
	float buttonHeight = 20.f;
	Vec2 centerPos = Vec2( 100.f, 80.f );
	UIButton* startButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos, Vec2( buttonWidth, buttonHeight ), "START" );
	UIButton* settingButton = UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -25.f ), Vec2( buttonWidth, buttonHeight ), "SETTING" );
	UIButton* quitButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -50.f ), Vec2( buttonWidth, buttonHeight ), "QUIT" );
	UIButton* volumeLowButton		= UIButton::CreateButtonWithPosSizeAndText( centerPos, Vec2( buttonWidth, buttonHeight ), "VOLUME LOW" );
	UIButton* volumeMediumButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -25.f ), Vec2( buttonWidth, buttonHeight ), "VOLUME MEDIUM" );
	UIButton* volumeHighButton		= UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -50.f ), Vec2( buttonWidth, buttonHeight ), "VOLUME HIGH" );

	UIButton* resumeButton		= UIButton::CreateButtonWithPosSizeAndText( centerPos, Vec2( buttonWidth, buttonHeight ), "RESUME" );
	UIButton* quitToMainButton	= UIButton::CreateButtonWithPosSizeAndText( centerPos + Vec2( 0.f, -35.f ), Vec2( buttonWidth, buttonHeight ), "QUIT" );


	startButton->SetEventName( "StartGame" );
	settingButton->SetEventName( "LoadSetting" );
	quitButton->SetEventName( "QuitGame" );

	volumeLowButton->SetEventName( "LowVolume" );
	volumeHighButton->SetEventName( "HighVolume" );
	volumeMediumButton->SetEventName( "MediumVolume" );

	resumeButton->SetEventName( "Resume" );
	quitToMainButton->SetEventName( "QuitToMain" );


	g_theEventSystem->SubscribeMethodToEvent( "StartGame", this, &Game::StartGame );
	g_theEventSystem->SubscribeMethodToEvent( "QuitGame", this, &Game::QuitGame );
	g_theEventSystem->SubscribeMethodToEvent( "LoadSetting", this, &Game::LoadSettings );

	g_theEventSystem->SubscribeMethodToEvent( "LowVolume", this, &Game::SetVolumeLow );
	g_theEventSystem->SubscribeMethodToEvent( "HighVolume", this, &Game::SetVolumeHigh );
	g_theEventSystem->SubscribeMethodToEvent( "MediumVolume", this, &Game::SetVolumeMedium );

	g_theEventSystem->SubscribeMethodToEvent( "Resume", this, &Game::Resume );
	g_theEventSystem->SubscribeMethodToEvent( "QuitToMain", this, &Game::QuitToMain );

	startButton->SetIsSelecting( true );
	m_buttons.push_back( startButton );
	m_buttons.push_back( settingButton );
	m_buttons.push_back( quitButton );

	volumeLowButton->SetIsSelecting( true );
	m_settingButtons.push_back( volumeLowButton );
	m_settingButtons.push_back( volumeMediumButton );
	m_settingButtons.push_back( volumeHighButton );

	resumeButton->SetIsSelecting( true );
	m_pauseButtons.push_back( resumeButton );
	m_pauseButtons.push_back( quitToMainButton );
}

void Game::UpdatePauseButtons()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_UP_ARROW ) ) {
		m_pauseButtons[0]->SetIsSelecting( true );
		m_pauseButtons[1]->SetIsSelecting( false );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_DOWN_ARROW ) ) {
		m_pauseButtons[0]->SetIsSelecting( false );
		m_pauseButtons[1]->SetIsSelecting( true );
	}
	else if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ENTER ) ) {
		for( int i = 0; i < m_pauseButtons.size(); i++ ) {
			if( m_pauseButtons[i]->GetIsSelecting() ) {
				m_pauseButtons[i]->ExecuteTargetFunction();
			}
		}
	}

	for( int i = 0; i < m_pauseButtons.size(); i++ ) {
		m_pauseButtons[i]->UpdateButton();
	}
}

void Game::RenderPauseButtons() const
{
	for( int i = 0; i < m_pauseButtons.size(); i++ ) {
		m_pauseButtons[i]->RenderButton();
	}
}

bool Game::StartGame( EventArgs& args )
{
	m_gameState = IN_GAME;
	StartGame();
	return true;
}

bool Game::QuitGame( EventArgs& args )
{
	m_isAppQuit = true;
	return true;
}

bool Game::LoadSettings( EventArgs& args )
{
	m_isOnSettingPage = true;
	return true;
}

bool Game::DebugGameInfo( EventArgs& args )
{
	std::string result = args.GetValue( "message", "" );
	DebugAddScreenText( Vec4( 0.0f, 0.8f, 0.f, 0.f ), Vec2::ZERO, 3.f, Rgba8::RED, Rgba8::RED, 2.f, result );
	return true;
}

bool Game::QuitToMain( EventArgs& args )
{
	m_gameState = ATTRACT_SCREEN;
	m_showPauseMenu = false;
	g_theApp->UnPauseGame();
	return true;
}

bool Game::Resume( EventArgs& args )
{
	m_showPauseMenu = false;
	g_theApp->UnPauseGame();
	return true;
}

bool Game::SetVolumeHigh( EventArgs& args )
{
	m_volume = 0.7f;
	m_isOnSettingPage = false;
	g_theAudioSystem->SetSoundPlaybackVolume( m_gameBGMID, m_volume * m_backGroundVolumeScale );
	return true;
}

bool Game::SetVolumeLow( EventArgs& args )
{
	m_volume = 0.3f;
	m_isOnSettingPage = false;
	g_theAudioSystem->SetSoundPlaybackVolume( m_gameBGMID, m_volume * m_backGroundVolumeScale );
	return true;
}

bool Game::SetVolumeMedium( EventArgs& args )
{
	m_volume = 0.5f;
	m_isOnSettingPage = false;
	g_theAudioSystem->SetSoundPlaybackVolume( m_gameBGMID, m_volume * m_backGroundVolumeScale );
	return true;
}

bool Game::BackToMain( EventArgs& args )
{
	m_isOnSettingPage = false;
	return true;
}

