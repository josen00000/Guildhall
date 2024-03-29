#include<iostream>
#include<stdio.h>
#include<windows.h>
#include "App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"

App* g_theApp=nullptr;
RenderContext* g_theRenderer=nullptr;
InputSystem* g_theInputSystem=nullptr;
bool ifPause=false;
Camera* g_camera=nullptr;
Camera* g_UICamera=nullptr;


void App::Startup()
{
	g_camera= new Camera( Vec2( 0, 0 ), Vec2( 200, 100 ) );
	g_UICamera=new Camera( Vec2( 200, 100 ), Vec2( 400, 200 ) );
	g_theRenderer=new RenderContext();
	g_theInputSystem=new InputSystem();
	m_theGame=new Game( g_theRenderer,  g_theInputSystem, m_isDevelopMode,g_camera );
	
	
	g_theRenderer->StartUp();
	m_theGame->Startup();
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
	timeLastFrameStarted=timeThisFrameStarted;
	
	BeginFrame(); //only for engine. not for game(for all engine system)
	Update(m_deltaTime);// for game
	Render();		//for game
	EndFrame();//only for engine. not for game(for all engine system)

}

bool App::HandleKeyPressed( unsigned char keyCode )
{
	if( keyCode == VK_ESCAPE ) // #SD1ToDo: move this "check for ESC pressed" code to App
	{
		g_theApp->HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	else if( keyCode==(unsigned char)'P' ) {
		ifPause=!ifPause;
		if( ifPause ) {
			m_timeFraction=0;
		}
		else {
			m_timeFraction=1;
		}
	}
	else if( keyCode==(unsigned char)'T' ) {
		m_timeFraction=0.1f;
	}
	else if(keyCode==VK_F8){
		m_theGame->Shutdown();
		delete m_theGame;
		
		m_theGame=new Game( g_theRenderer,g_theInputSystem,m_isDevelopMode,g_camera );
		m_theGame->Startup();
	}
	else if(keyCode==VK_F1){
		m_isDevelopMode=!m_isDevelopMode;
		m_theGame->m_developMode=m_isDevelopMode;
	}
	else {
		m_theGame->HandlePlayerInputPressed(keyCode);
	}
		
		
	return true;
}

bool App::HandleKeyReleased( unsigned char keyCode )
{
	if( keyCode==(unsigned char)'T' ) {
		m_timeFraction=1;
	}
	else{
		m_theGame->HandlePlayerInputReleased(keyCode);
	}
	return true;
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

void App::BeginFrame()
{
	//g_theInputSystem->BeginFrame();
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
	m_theGame->Render();
	
	g_theRenderer->BeginCamera(*g_UICamera);
	m_theGame->RenderUI();
	
}

void App::EndFrame()
{

}






