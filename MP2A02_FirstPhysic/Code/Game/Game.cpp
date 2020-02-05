#include "Game.hpp"
#include "Game/App.hpp"
#include "Game/GameObject.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern App*				g_theApp;
extern BitmapFont*		g_squirrelFont;
extern AudioSystem*		g_theAudioSystem; 
extern RenderContext*	g_theRenderer;
extern InputSystem*		g_theInputSystem;


Game::Game( Camera* gameCamera )
	:m_gameCamera(gameCamera)
{
	m_rng = new RandomNumberGenerator( 0 );
}

void Game::Startup()
{
	CreateGameObject();
}

void Game::Shutdown()
{
	delete m_rng;
}

void Game::RunFrame( float deltaSeconds )
{
	Update( deltaSeconds );
	UpdateMousePos();
}

void Game::EndFrame()
{

}

void Game::Reset()
{
	Startup();
}

void Game::Update( float deltaSeconds )
{
	CheckIfExit();
		
}

void Game::CheckIfExit()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC ) ){
		g_theApp->HandleQuitRequested();
	}
}

void Game::UpdateMousePos()
{
	// Get Mouse Pos;
	m_mousePos = g_theInputSystem->GetNormalizedMousePosInCamera( *m_gameCamera );
}

void Game::RenderMouse() const
{
	g_theRenderer->DrawCircle( m_mousePos, 1, 1, Rgba8::WHITE );
}

void Game::Render() const
{
	//RenderMouse();
	RenderGameObjects();
}


void Game::RenderGameObjects() const
{
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		m_gameObjects[objIndex]->Render();
	}
}

void Game::CreateGameObject()
{
	// get cursor position
	// create the game object 
	Vec2 cursePos = g_theInputSystem->GetNormalizedMousePos();
	float tempRadius = m_rng->RollRandomIntInRange( 1, 10 );
	GameObject* tempGameObj = new GameObject( cursePos, tempRadius );
	for( int objIndex = 0; objIndex < m_gameObjects.size(); objIndex++ ) {
		if( m_gameObjects[objIndex] == nullptr ) {
			m_gameObjects[objIndex] = tempGameObj;
			return;
		}
	}
	m_gameObjects.push_back( tempGameObj );
}

void Game::DeleteGameObject()
{

}

