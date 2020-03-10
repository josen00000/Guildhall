#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
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
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_theEventSystem	= new EventSystem();

	
	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theInputSystem->Startup();

	g_camera			= Camera::CreatePerspectiveCamera( 60, -0.1f, -100.f );
	g_camera->SetClearMode( 0, Rgba8::DARK_GRAY );
	g_camera->EnableClearColor( Rgba8::DARK_GRAY );
	g_camera->EnableClearDepth( 1 );
	g_UICamera			= new Camera( Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_devCamera			= new Camera( Vec2( -32, -18 ), Vec2( 32, 18 ) );
	g_theGame			= new Game( g_camera, g_UICamera );

	g_theGame->Startup();
	g_theInputSystem->SetCursorMode( CURSOR_RELATIVE );
	g_squirrelFont		= g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole		= DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );
	g_theConsole->PrintString( Rgba8::RED, std::string( "test ") );
	g_theConsole->Startup();

	std::string helpComd = std::string( "help" ); 
	std::string quitComd = std::string( "quit" );
	std::string testComd = std::string( "test" );
	std::string testComd1 = std::string( "test1" );
	std::string testComd2 = std::string( "test2" );
	
	std::string helpComdDesc = std::string( "List all commands in devConsole." );
	std::string quitComdDesc = std::string( "Quit the app." );
	std::string testComdDesc = std::string( "test for command history1." );
	std::string testComd1Desc = std::string( "test for command history1." );
	std::string testComd2Desc = std::string( "test for command history1." );

	EventCallbackFunctionPtr helpFuncPtr = HelpCommandEvent;
	EventCallbackFunctionPtr quitFuncPtr = QuitCommandEvent;
	g_theConsole->AddCommandToCommandList( helpComd, helpComdDesc, helpFuncPtr );
	g_theConsole->AddCommandToCommandList( quitComd, quitComdDesc, quitFuncPtr );
	g_theConsole->AddCommandToCommandList( testComd, testComdDesc, nullptr );
	g_theConsole->AddCommandToCommandList( testComd1, testComd1Desc, nullptr );
	g_theConsole->AddCommandToCommandList( testComd2, testComd2Desc, nullptr );
}

void App::Shutdown()
{
	g_theGame->Shutdown();
	g_theInputSystem->Shutdown();
	g_theConsole->Shutdown();
	g_theRenderer->Shutdown();
	
	delete g_camera;
	delete g_UICamera;
	delete g_devCamera;
	delete g_theInputSystem;
	delete g_theGame;
	delete g_theEventSystem;
	delete g_theRenderer;
	
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
	g_theInputSystem->Update();

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
	
	g_theRenderer->BeginCamera( *g_devCamera );
	g_theConsole->Render( *g_theRenderer );
	g_theRenderer->EndCamera();

	
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
	for( auto it = DevConsole::s_commands.begin(); it != DevConsole::s_commands.end(); ++it ){
		std::string comd = it->first;
		std::string displayString = comd + ": " + it->second;
		g_theConsole->PrintString( g_theConsole->m_defaultColor, displayString );
	}
	return true;
}