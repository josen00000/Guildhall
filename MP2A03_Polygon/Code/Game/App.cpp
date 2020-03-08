#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Physics2D.hpp"

// Game
App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;

// Engine
BitmapFont*		g_squirrelFont		= nullptr;
Camera*			g_camera			= nullptr;
Camera*			g_UICamera			= nullptr;
Camera*			g_devCamera			= nullptr;
DevConsole*		g_theConsole		= nullptr;
EventSystem*	g_theEventSystem	= nullptr;
InputSystem*	g_theInputSystem	= nullptr;
Physics2D*		g_thePhysics		= nullptr;
RenderContext*	g_theRenderer		= nullptr;

void App::Startup()
{
	StartupStage1();
	StartupStage2();
	StartupStage3();
}

void App::StartupStage1()
{
	// current unused
}

void App::StartupStage2()
{
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_thePhysics		= new Physics2D();
	g_devCamera			= new Camera( Vec2( DEV_CAMERA_MIN_X, DEV_CAMERA_MIN_Y ), Vec2( DEV_CAMERA_MAX_X, DEV_CAMERA_MAX_Y ) );

	g_theInputSystem->Startup();
	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
}

void App::StartupStage3()
{
	g_UICamera			= new Camera( Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_camera			= new Camera( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_theGame			= new Game( g_camera, g_UICamera );
	g_squirrelFont		= g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole		= DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );

	g_theConsole->Startup();
	g_theGame->Startup();
}

void App::Shutdown()
{
	g_theRenderer->Shutdown();
	g_theInputSystem->Shutdown();
	g_theGame->Shutdown();
	g_theConsole->Shutdown();
	
	delete g_theInputSystem;
	delete g_theGame;
	delete g_theRenderer;
	delete g_UICamera;
	delete g_devCamera;
	delete g_theEventSystem;

	
	g_theGame			= nullptr;
	g_UICamera			= nullptr;
	g_squirrelFont		= nullptr;
	g_theRenderer		= nullptr;
	g_theInputSystem	= nullptr;
	g_theRenderer		= nullptr;
	
}

void App::RunFrame()
{
	static double timeLastFrameStarted = GetCurrentTimeSeconds();
	double timeThisFrameStarted = GetCurrentTimeSeconds();
	double deltaSeconds = timeThisFrameStarted - timeLastFrameStarted;
	m_deltaTime = (float)deltaSeconds * m_timeFraction;
	//m_deltaTime=0.016;//testing
	timeLastFrameStarted = timeThisFrameStarted;
	if(g_theInputSystem->IsKeyDown(KEYBOARD_BUTTON_ID_T)){
		m_deltaTime /= 10;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_Y ) ) {
		m_deltaTime *= 4;
	}	
	BeginFrame();
	Update(m_deltaTime);
	Render();
	EndFrame();

}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

void App::CheckGameQuit()
{
	if(g_theGame->m_isAppQuit){
		m_isQuitting = true;
	}
}

void App::HandleKeyPressed( unsigned char inValue )
{
	g_theInputSystem->UpdateKeyBoardButton( inValue, true );
}

void App::HandleKeyReleased( unsigned char inValue )
{
	g_theInputSystem->UpdateKeyBoardButton( inValue, false );
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
	g_theGame->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	g_theRenderer->UpdateFrameTime( deltaSeconds );
	g_theGame->RunFrame( deltaSeconds );
	CheckGameQuit();
}

const void App::Render() const
{
	g_theGame->Render();
	
	g_theGame->RenderUI();
	g_theConsole->Render( *g_theRenderer );
}

void App::EndFrame()
{
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
}






