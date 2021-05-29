#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <fstream>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "../../Thesis/Code/Game/Camera/CameraSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "ThirdParty/imgui/imgui.h"
#include "ThirdParty/imgui/imgui_impl_dx11.h"
#include "ThirdParty/imgui/imgui_impl_win32.h"

// Game
App*			g_theApp			= nullptr;
Game*			g_theGame			= nullptr;
CameraSystem*	g_theCameraSystem	= nullptr;

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
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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
	g_squirrelFont		= g_theRenderer->CreateOrGetBitmapFontFromFile( "testing", "Data/Fonts/SquirrelFixedFont" );
	g_theConsole = DevConsole::InitialDevConsole( g_squirrelFont, g_devCamera );

	g_theGame->Startup();
	g_theConsole->Startup();

	// imgui setup
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	//ImGuiIO& io = ImGui::GetIO();
	HWND topWindow = (HWND)g_theWindow->GetTopWindowHandle();
	ImGui_ImplWin32_Init( topWindow );
	ImGui_ImplDX11_Init( g_theRenderer->GetDevice(), g_theRenderer->GetContext() );
	ImGui::StyleColorsDark();
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
	BeginFrame();
	//Update( 0.016f );
	Update( Clock::GetMasterDeltaSeconds() );
	Render();
	EndFrame();
}

void App::HandleQuitRequested()
{
	m_isQuitting = true;
}

void App::CheckGameQuit()
{
	if( g_theGame->m_isAppQuit ){
		m_isQuitting = true;
	}
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

void App::handleIMGUIInput( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	ImGui_ImplWin32_WndProcHandler( (HWND)windowHandle, wmMessageCode, wParam, lParam );
}

void App::BeginFrame()
{
	Clock::BeginFrame();
	g_theInputSystem->BeginFrame();
	g_thePhysics->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudioSystem->BeginFrame();
	g_theCameraSystem->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	g_theEventSystem->Update();
	g_theGame->RunFrame( deltaSeconds );
	g_theConsole->Update( deltaSeconds );
	CheckGameQuit();
}

const void App::Render() const
{
	g_theCameraSystem->Render();

	g_theRenderer->BeginCamera( g_UICamera );
	//g_theGame->RenderUI();
	g_theRenderer->EndCamera();

	g_theConsole->Render( *g_theRenderer );
	// debug render
	DebugRenderScreenTo( g_gameCamera->GetColorTarget() );
	DebugRenderWorldToCamera( g_gameCamera );
}

void App::EndFrame()
{
	g_theCameraSystem->EndFrame();
	g_theGame->EndFrame();
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudioSystem->EndFrame();
	g_theConsole->EndFrame();
	DebugRenderEndFrame();
	//float deltaSeconds =  Clock::GetMasterDeltaSeconds();
// 	if( deltaSeconds < 0.016f ) {
// 		Sleep( (0.016f - deltaSeconds) * 1000.f );
// 	}
}






