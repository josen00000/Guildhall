#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Physics2D.hpp"

// Game
App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;

// Engine
BitmapFont*		g_squirrelFont		= nullptr;
Camera*			g_gameCamera		= nullptr;
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
// 	g_gameCamera		= new Camera( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ), 0.9f );
// 	g_UICamera			= new Camera( Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
// 	g_devCamera			= new Camera( Vec2( 0, 0 ), Vec2( 30, 20 ) );
// 	g_theRenderer		= new RenderContext();
// 	g_theInputSystem	= new InputSystem();
// 	g_theGame			= new Game( g_gameCamera, g_UICamera );
// 	g_thePhysics		= new Physics2D();
// 	
// 	g_theWindow->SetInputSystem( g_theInputSystem );
// 	g_theRenderer->StartUp( g_theWindow );
// 	DebugRenderSystemStartup( g_theRenderer );
// 	g_theGame->Startup();
// 	g_theInputSystem->Startup();
// 	
// 	g_thePhysics->StartUp();
// 	g_theConsole = DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );
// 	g_theConsole->Startup();

}

void App::StartupStage1()
{
	// memory system and log system
	Clock::SystemStartUp();
}

void App::StartupStage2()
{
	// initialize system
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_thePhysics		= new Physics2D();
	g_theEventSystem	= new EventSystem();

	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theInputSystem->Startup();
	g_thePhysics->StartUp();
	EnableDebugRendering();
}

void App::StartupStage3()
{
	g_gameCamera		= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_UICamera			= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_devCamera			= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( 0, 0 ), Vec2( 30, 20 ) );

	DebugRenderSystemStartup( g_theRenderer, g_gameCamera );
	g_theGame			= new Game( g_gameCamera, g_UICamera );
	g_theGame->Startup();

	g_theConsole = DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );
	g_theConsole->Startup();

}

void App::Shutdown()
{
	g_theRenderer->Shutdown();
	g_theInputSystem->Shutdown();
	g_theGame->Shutdown();
	g_theConsole->Shutdown();
	DebugRenderSystemShutdown();
	Clock::SystemShutDown();
	
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
	g_theEventSystem	= nullptr;
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
	Clock::BeginFrame();
	g_theInputSystem->BeginFrame();
	g_thePhysics->BeginFrame();
	g_theRenderer->BeginFrame();
	// Debug timer error
 	float currentTime1 = g_theRenderer->m_clock->GetTotalSeconds();
 	float currentTime2 = g_thePhysics->m_clock->GetTotalSeconds();
 	float deltaTime1 = g_theRenderer->m_clock->GetLastDeltaSeconds();
 	float deltaTime2 = g_thePhysics->m_clock->GetLastDeltaSeconds();
 	std::string clockString1 =  "render time is " + std::to_string( currentTime1 ) + "   delta time is " + std::to_string( deltaTime1 );
 	//DebugAddScreenText( Vec4( 0.4f, 0.4f, 10.f, 10.f ), Vec2::ZERO, 3.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, clockString1 );
 	std::string clockString2 =  "time is " + std::to_string( currentTime2 ) + "  delta time is " + std::to_string( deltaTime2 );
 	//DebugAddScreenText( Vec4( 0.4f, 0.4f, 10.f, 15.f ), Vec2::ZERO, 3.f, Rgba8::GREEN, Rgba8::GREEN, 0.f, clockString2 );
}

void App::Update( float deltaSeconds )
{
	
	g_theGame->RunFrame( deltaSeconds );
	CheckGameQuit();
}

const void App::Render() const
{
	g_gameCamera->m_clearColor = Rgba8::DARK_GRAY;
	g_theRenderer->BeginCamera( g_gameCamera );
	g_theGame->Render();
	g_theRenderer->EndCamera();

	// debug render
	DebugRenderScreenTo( g_gameCamera->GetColorTarget() );
	DebugRenderWorldToCamera( g_gameCamera );

	g_theRenderer->BeginCamera( g_UICamera );
	g_theGame->RenderUI();
	g_theRenderer->EndCamera();

	g_theRenderer->BeginCamera( g_devCamera );
	g_theConsole->Render( *g_theRenderer );
	g_theRenderer->EndCamera();
}

void App::EndFrame()
{
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	DebugRenderEndFrame();
}






