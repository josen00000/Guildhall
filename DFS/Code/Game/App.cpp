#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Audio/AudioSystem.hpp"

// Game
App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;

// Engine
BitmapFont*		g_squirrelFont		= nullptr;
Camera*			g_gameCamera		= nullptr;
Camera*			g_debugCamera		= nullptr;
Camera*			g_UICamera			= nullptr;
Camera*			g_devCamera			= nullptr;
DevConsole*		g_theConsole		= nullptr;
EventSystem*	g_theEventSystem	= nullptr;
InputSystem*	g_theInputSystem	= nullptr;
AudioSystem*	g_theAudioSystem	= nullptr;
Physics2D*		g_thePhysics		= nullptr;
RenderContext*	g_theRenderer		= nullptr;

extern Convention g_convention;

void App::Startup()
{
	StartupStage1();
	StartupStage2();
	StartupStage3();
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
	g_theAudioSystem	= new AudioSystem();
	g_thePhysics		= new Physics2D();
	g_theEventSystem	= new EventSystem();

	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theInputSystem->Startup();
	g_thePhysics->StartUp();
	EnableDebugRendering();
	g_theInputSystem->HideSystemCursor();
	g_convention = X_RIGHT_Y_UP_Z_BACKWARD;
}

void App::StartupStage3()
{
	g_gameCamera		= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_debugCamera		= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_UICamera			= Camera::CreateOrthographicCamera( g_theRenderer, Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) ); 
	g_devCamera			= Camera::CreateOrthographicCamera( g_theRenderer, Vec2::ZERO, Vec2( 30, 20 ) ); 

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
	timeLastFrameStarted = timeThisFrameStarted;
	if(g_theInputSystem->IsKeyDown(KEYBOARD_BUTTON_ID_T)){
		m_deltaTime /= 10;
	}
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_Y ) ) {
		m_deltaTime *= 4;
	}	
	if( m_isPauseTime ) {
		m_deltaTime = 0.f;
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

void App::PauseGame()
{
	m_isPauseTime = true;
}

void App::UnPauseGame()
{
	m_isPauseTime = false;
}

void App::BeginFrame()
{
	Clock::BeginFrame();
	g_theInputSystem->BeginFrame();
	g_thePhysics->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudioSystem->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	
	g_theGame->RunFrame( deltaSeconds );
	CheckGameQuit();
}

const void App::Render() const
{
	g_gameCamera->EnableClearColor( Rgba8::DARK_GRAY );
	g_theRenderer->BeginCamera( g_gameCamera );
	g_theGame->RenderGame();
	g_theRenderer->EndCamera();


	g_theRenderer->BeginCamera( g_UICamera );
	g_theGame->RenderUI();
	g_theRenderer->EndCamera();

	// debug render
	DebugRenderScreenTo( g_gameCamera->GetColorTarget() );
	DebugRenderWorldToCamera( g_gameCamera );

// 	g_theRenderer->BeginCamera( g_devCamera );
// 	g_theConsole->Render( *g_theRenderer );
// 	g_theRenderer->EndCamera();
}

void App::EndFrame()
{
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudioSystem->EndFrame();
	DebugRenderEndFrame();
}






