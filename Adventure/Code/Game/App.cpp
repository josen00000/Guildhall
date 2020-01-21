#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "engine/Renderer/BitmapFont.hpp"




App* g_theApp=nullptr;
Game* g_theGame=nullptr;
RenderContext* g_theRenderer = nullptr;
BitmapFont* g_squirrelFont = nullptr;
InputSystem* g_theInputSystem = nullptr;
AudioSystem* g_theAudioSystem = nullptr;
bool ifPause = false;
Camera* g_camera = nullptr;
Camera* g_UICamera = nullptr;
Camera* g_consoleCamera = nullptr;

void App::Startup()
{
	
	g_camera			= new Camera( Vec2( 0, 0 ), Vec2( 16, 9 ) );
	g_UICamera			= new Camera( Vec2( 0, 0 ), Vec2( 80, 45 ) );
	g_consoleCamera		= new Camera( Vec2(10, 10), Vec2(30, 30));
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_theAudioSystem	= new AudioSystem();
	g_theGame			= new Game( m_isDevelopMode,g_camera );
	
	m_theGame = g_theGame;
	g_theRenderer->StartUp();
	g_squirrelFont = g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );


}

void App::Shutdown()
{

	g_theRenderer->Shutdown();
	g_theRenderer=nullptr;
	m_theGame->Shutdown();
	m_theGame=nullptr;
	
}

void App::RunFrame()
{
	static double timeLastFrameStarted=GetCurrentTimeSeconds();
	double timeThisFrameStarted=GetCurrentTimeSeconds();
	double deltaSeconds=timeThisFrameStarted-timeLastFrameStarted;
	m_deltaTime=(float)deltaSeconds*m_timeFraction;
	//m_deltaTime=0.016f;//testing
	timeLastFrameStarted=timeThisFrameStarted;
	if(g_theInputSystem->IsKeyDown(KEYBOARD_BUTTON_ID_T)){
		m_deltaTime/=10;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_Y ) ) {
		m_deltaTime*=4;
	}	
	BeginFrame();
	Update(m_deltaTime);
	Render();
	EndFrame();

}





void App::HandleQuitRequested()
{
	m_isQuitting=true;
}

void App::CheckGameQuit()
{
	if(m_theGame->isAppQuit){
		m_isQuitting=true;
	}
}

void App::HandleKeyPressed( unsigned char inValue )
{
	g_theInputSystem->UpdateKeyBoardButton(inValue, true);
}

void App::HandleKeyReleased( unsigned char inValue )
{
	g_theInputSystem->UpdateKeyBoardButton(inValue, false);
}

void App::ResetGame()
{
	g_theGame->Shutdown();
	delete g_theGame;
	Startup();
}

void App::BeginFrame()
{
	g_theInputSystem->BeginFrame();
	g_theAudioSystem->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	m_theGame->RunFrame(deltaSeconds);
	CheckGameQuit();
}

const void App::Render() const
{
	g_theRenderer->ClearScreen( Rgba8(0,0,0,255));
	g_theRenderer->BeginCamera(*g_camera);
	g_theRenderer->BeginView();
	g_theRenderer->EndCamera(*g_camera);
	//testi
	m_theGame->Render();
	
	if(m_theGame->m_gameState == GAME_STATE_PLAYING){
		g_theRenderer->BeginCamera(*g_UICamera);
		m_theGame->RenderUI();
		g_theRenderer->EndCamera(*g_UICamera);
	}

	
}

void App::EndFrame()
{
	g_theInputSystem->EndFrame();
	g_theAudioSystem->EndFrame();
}






