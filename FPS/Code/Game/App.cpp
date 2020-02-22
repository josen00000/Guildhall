#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"


App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;
Camera*			g_camera			= nullptr;
Camera*			g_UICamera			= nullptr;
Camera*			g_devCamera			= nullptr;
BitmapFont*		g_squirrelFont		= nullptr;
RenderContext*	g_theRenderer		= nullptr;
InputSystem*	g_theInputSystem	= nullptr;
EventSystem*	g_theEventSystem	= nullptr;
DevConsole*		g_theConsole		= nullptr;




void App::Startup()
{
	g_camera			= new Camera( Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	g_UICamera			= new Camera( Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_devCamera			= new Camera( Vec2( -32, -18 ), Vec2( 32, 18 ) );
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_theGame			= new Game( g_camera, g_UICamera );
	g_theEventSystem	= new EventSystem();
	
	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theGame->Startup();


	g_squirrelFont		= g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole		= DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );
	g_theConsole->PrintString( Rgba8::RED, std::string( "test ") );
	g_theConsole->Startup();

	Command helpComd = Command( "help", "List all commands in devConsole." );
	Command quitComd = Command( "quit", "Quit the app." );
	Command testComd = Command( "test", "test for command history1." );
	Command testComd1 = Command( "test1", "test for command history1." );
	Command testComd2 = Command( "test2", "test for command history1." );



	EventCallbackFunctionPtr helpFuncPtr = HelpCommandEvent;
	EventCallbackFunctionPtr quitFuncPtr = QuitCommandEvent;
	g_theConsole->AddCommandToCommandList( helpComd, helpFuncPtr );
	g_theConsole->AddCommandToCommandList( quitComd, quitFuncPtr );
	g_theConsole->AddCommandToCommandList( testComd, nullptr );
	g_theConsole->AddCommandToCommandList( testComd1, nullptr );
	g_theConsole->AddCommandToCommandList( testComd2, nullptr );
}

void App::Shutdown()
{
	g_theRenderer->Shutdown();
	g_theInputSystem->Shutdown();
	g_theGame->Shutdown();
	g_theConsole->Shutdown();
	
	delete g_camera;
	delete g_UICamera;
	delete g_theInputSystem;
	delete g_theGame;
	delete g_devCamera;
	delete g_theEventSystem;
	//delete g_theRenderer;
	
	g_theGame			= nullptr;
	g_camera			= nullptr;
	g_UICamera			= nullptr;
	g_squirrelFont		= nullptr;
	g_theInputSystem	= nullptr;
	g_devCamera			= nullptr;
	g_theEventSystem	= nullptr;
	//g_theRenderer		= nullptr;
	
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
	Update( m_deltaTime );
	Render();
	EndFrame();

}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

void App::HandleDevConsoleInput()
{
	

}

void App::CheckGameQuit()
{
	if(g_theGame->m_isAppQuit){
		m_isQuitting = true;
	}
	if( g_theConsole->m_ableQuit ) {
		m_isQuitting = true;
	}
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
}

void App::Update( float deltaSeconds )
{
	g_theRenderer->UpdateFrameTime( deltaSeconds );
	g_theGame->RunFrame( deltaSeconds );
	CheckGameQuit();

	// devconsole
	HandleDevConsoleInput();
	UpdateDevConsole( deltaSeconds );
	
}

void App::UpdateDevConsole( float deltaSeconds )
{
	g_theConsole->Update( deltaSeconds );
}

const void App::Render() const
{


	g_theGame->Render();
	
	//g_theRenderer->BeginCamera(*g_UICamera);
	//g_theGame->RenderUI();
	
	g_theConsole->Render( *g_theRenderer );

	
}

void App::EndFrame()
{
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theConsole->EndFrame();
}


bool QuitCommandEvent( EventArgs& args )
{
	UNUSED(args);
	g_theApp->HandleQuitRequested();
	return true;
}


bool HelpCommandEvent( EventArgs& args )
{
	UNUSED(args);
	for( int comdIndex = 0; comdIndex < DevConsole::s_commands.size(); comdIndex++ ) {
		Command comd = DevConsole::s_commands[comdIndex];
		std::string displayString = comd.body + ":  " + comd.desc;
		g_theConsole->PrintString( g_theConsole->m_defaultColor, displayString );
	}
	return true;
}