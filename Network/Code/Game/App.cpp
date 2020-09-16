#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Delegate.hpp"
#include "Engine/Core/NamedProperties.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Job/JobSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Network/NetworkSystem.hpp"


App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;
Camera*			g_camera			= nullptr;
Camera*			g_UICamera			= nullptr;
Camera*			g_devCamera			= nullptr;
BitmapFont*		g_squirrelFont		= nullptr;
RenderContext*	g_theRenderer		= nullptr;
InputSystem*	g_theInputSystem	= nullptr;
EventSystem*	g_theEventSystem	= nullptr;
AudioSystem*	g_theAudioSystem	= nullptr;
NetworkSystem*	g_theNetworkSystem	= nullptr;
JobSystem*		g_theJobSystem		= nullptr;
DevConsole*		g_theConsole		= nullptr;
//Convention		g_convention;

#include <vector>
#include <functional>



void App::Startup()
{
	//StringTesting();
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_theEventSystem	= new EventSystem();
	g_theAudioSystem	= new AudioSystem();
	//TemplateTesting();

	Clock::SystemStartUp();
	m_clock = new Clock();
	
	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theInputSystem->Startup();

	g_gameConfigBlackboard.PopulateFromXmlFile( "Data/GameConfig.xml" );
	m_windowName = g_gameConfigBlackboard.GetValue( "WindowName", "default name " );
	HWND hWnd = (HWND)g_theWindow->GetHandle();
	SetWindowTextA( hWnd, m_windowName.c_str() );


	g_camera			= Camera::CreatePerspectiveCamera( g_theRenderer, 60, -0.09f, -100.f );
	g_camera->m_debugString = "gamecamera";
	g_camera->SetClearMode( CLEAR_NONE, Rgba8::DARK_GRAY );
	g_camera->EnableClearColor( Rgba8::DARK_GRAY );
	g_camera->EnableClearDepth( 1 );
	g_camera->SetRenderContext( g_theRenderer );

	g_UICamera			= new Camera( g_theRenderer, Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_devCamera			= new Camera( g_theRenderer, Vec2( -32, -18 ), Vec2( 32, 18 ) );

	g_squirrelFont		= g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole		= DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );
	g_theConsole->Startup();

	g_theNetworkSystem = new NetworkSystem;
	g_theNetworkSystem->StartUp();

	g_theJobSystem = new JobSystem();
	g_theJobSystem->CreateWorkerThread( 8 );

	g_theGame			= new Game( g_camera, g_UICamera );
	g_theGame->SetConvention( X_FORWARD_Y_LEFT_Z_UP ); // basis currently not use
	g_convention = X_RIGHT_Y_UP_Z_BACKWARD;

	g_theGame->Startup();
	EnableDebugRendering();
	g_theInputSystem->SetCursorMode( CURSOR_RELATIVE );

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

void App::TemplateTesting()
{
// 	TestClass obj( 7 );
// 	g_theEventSystem->SubscribeEvent( "test", eventTest );
// 	g_theEventSystem->SubscribeMethodToEvent( "test1", &obj, &TestClass::EventTest );
// 	EventArgs args;
// 	args.SetValue<float>( "float test", 1 );
// 	args.SetValue<int>( "int test", 10 );
// 	g_theEventSystem->FireEvent( "test", args );
// 	g_theEventSystem->FireEvent( "test1", args );
// 
// 	Delegate<int, int> intDele;
// 	intDele.subscribe_method( &obj, &TestClass::SomeMethod );
// 	//intDele.subscribe_method( &obj, &TestClass::SomeMethod1 );

}

void App::StringTesting()
{
	std::string test1 = "a      b     c d s fffff   eeewwa w wedfasdf    wer  ";
	std::string test2 = "abcdesf";
	std::string test3 = "abcdesf     ";
	std::string test4 = "we are the worldsssss rea   lly  ddd ?";
	std::string res1 = TrimStringWithOneSpace( test1 );
	std::string res2 = TrimStringWithOneSpace( test2 );
	std::string res3 = TrimStringWithOneSpace( test3 );
	std::string res4 = TrimStringWithOneSpace( test4 );
	
	//ObjectReader* test = new ObjectReader( "Data/Model/planet.obj" );
	//test->LoadObject();
// 	Vec3 a = test->m_positions[0];
// 	Vec3 b = test->m_positions.back();
// 	Vec3 c = test->m_normals[0];
// 	Vec3 d = test->m_normals.back();
// 	Vec2 a2 = test->m_uvs[0];
// 	Vec2 b2 = test->m_uvs.back();
// 	auto dddd = test->m_facePoints.back();
}

void App::NamedPropertyTesting()
{
// 	NamedProperties* testProp = new NamedProperties();
// 	float a = 10.f;
// 	Camera* cameraA = new Camera();
// 	Camera* cameraB = new Camera();
// 
// 
// 	testProp->SetValue<float>( "floatA", a );
// 	testProp->SetValue<Camera>( "cameraA", *cameraA );
// 	//testProp->SetValue<ObjectReader>( "ReaderA", *testReader );
// 
// 	float b = testProp->GetValue<float>( "floatA", 20.f );
// 	Camera cameraC = testProp->GetValue<Camera>( "cameraA", *cameraB );
	
}

void App::Shutdown()
{
	g_theGame->Shutdown();
	g_theInputSystem->Shutdown();
	g_theConsole->Shutdown();
	g_theJobSystem->Shutdown();

	Clock::SystemShutDown();
	
	delete g_camera;
	delete g_UICamera;
	delete g_devCamera;

	g_theRenderer->Shutdown();
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
	BeginFrame();
	Update( (float)m_clock->GetLastDeltaSeconds() );
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
	Clock::BeginFrame();
	g_theInputSystem->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_theNetworkSystem->BeginFrame();
	//g_theGame->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	//g_theRenderer->UpdateFrameTime( deltaSeconds );
	g_theNetworkSystem->Update( deltaSeconds );
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
	g_theGame->RenderGame();
	
	//g_theRenderer->BeginCamera(*g_UICamera);
	//g_theGame->RenderUI();
	
	g_theConsole->Render( *g_theRenderer );

}

void App::EndFrame()
{
	g_theRenderer->EndFrame();
	g_theInputSystem->EndFrame();
	g_theConsole->EndFrame();
	DebugRenderEndFrame();
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

/*
bool eventTest( EventArgs& args ) {
	float a = args.GetValue<float>( "float test", 0 );
	return true;
}*/