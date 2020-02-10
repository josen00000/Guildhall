#include "Game.hpp"
#include "Game/App.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
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
	m_isAppQuit		= false;
}

void Game::Shutdown()
{
	delete m_rng;
	delete m_gameCamera;
	delete m_UICamera;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
}

void Game::Update( float deltaSeconds )
{
	CheckIfExit();
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

void Game::Render() const
{
}

void Game::RenderUI() const
{
}

void Game::LoadAssets()
{
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
}

