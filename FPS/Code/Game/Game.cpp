#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/World.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;


Game::Game( Camera* gameCamera, Camera* UICamera )
	:m_gameCamera(gameCamera)
	,m_UICamera(UICamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	m_world = new World();
	m_gameState = GAME_STATE_LOADING;
	m_developMode	= false;
	m_noClip		= false;
	m_isPause		= false;
	m_debugCamera	= false;
	m_isAppQuit		= false;
	m_isAttractMode	= false;
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_world;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
}

void Game::Reset()
{
	delete m_world;
	m_world = nullptr;
	Startup();
}

void Game::Update( float deltaSeconds )
{
	CheckIfExit();
	switch ( m_gameState )
	{
		case GAME_STATE_LOADING:{
			LoadAssets();
			m_gameState = GAME_STATE_ATTRACT;
			break;
		}
		case GAME_STATE_ATTRACT:{
			static bool isResetFinished = false;
			if( !isResetFinished ){
				Reset();
				isResetFinished = true;
			}
			if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_SPACE)){
				m_gameState = GAME_STATE_PLAYING;
			}
			break;
		}
		case GAME_STATE_PLAYING:{
			if( m_isPause ){ return; }
			m_world->Update( deltaSeconds );
			break;
		}
		case GAME_STATE_END:{
			break;
		}


	}
}

void Game::UpdateUI( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::UpdateCamera( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
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
	m_world->Render();
}

void Game::RenderUI() const
{
	g_theRenderer->DrawVertexVector( m_UIVertices );
}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	//g_theRenderer->CreateOrGetTextureFromFile("FilePath");
	//g_theAudioSystem->CreateOrGetSound("FilePath");
}

