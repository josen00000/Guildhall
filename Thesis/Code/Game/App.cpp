#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <fstream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Camera/CameraSystem.hpp"
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
	// test
	std::vector<Vec2> testPoints;
	testPoints.emplace_back( 2.f,5.f );
	testPoints.emplace_back( -4.f, 3.f );
	testPoints.emplace_back( 5.f, 1.f );

	//Polygon2 testPoly = Polygon2::MakeConvexFromPointCloud( testPoints );
	//float area = testPoly.GetArea();
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
	Update( 0.016f );
	//Update( Clock::GetMasterDeltaSeconds() );
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

void App::SonyTest()
{
	// generate data
	RandomNumberGenerator* rng = new RandomNumberGenerator();
	std::string inputfileDataPath = "./Data/DevConsole/SonyTest.txt";
	std::string outputfileDataPath = "./Data/DevConsole/SonyTestOutput.txt";
	std::ofstream myInputFile;
	std::ofstream myOutputFile;
	myInputFile.open( inputfileDataPath );
	myOutputFile.open( outputfileDataPath );
	myInputFile << "total 10 set of data.\n ";
	for( int i = 0; i < 10; i++ ) {
		// 10 set of data test
		myInputFile << "Index :" << i  << std::endl;
		int totalPointsNum = rng->RollRandomIntInRange( 5, 50 );

		myInputFile << "total Points :" << totalPointsNum << std::endl;
		Vec2 minPoint = Vec2::ZERO;
		Vec2 maxPoint = Vec2( 50.f );
		std::vector<Vec2> tempPoints;
		// generate input
		for( int j = 0; j < totalPointsNum; j++ ) {
			Vec2 tempPoint = rng->RollRandomVec2InRange( minPoint, maxPoint );
			tempPoints.push_back( tempPoint ); 
			myInputFile << tempPoint.x << " " << tempPoint.y << std::endl;
		}
		myInputFile << std::endl;
		// generate ouput
		Polygon2 generatedPoly = Polygon2::MakeConvexFromPointCloud( tempPoints );
		std::vector<Vec2> worldPoints;
		generatedPoly.GetAllVerticesInWorld( worldPoints );
		myOutputFile << "Index :" << i  << std::endl;
		myOutputFile << "Ouput points : " << worldPoints.size() << std::endl;
		for( int j = 0; j < worldPoints.size(); j++ ) {
			myOutputFile << worldPoints[j].x << " " << worldPoints[j].y << std::endl;
		}
	}

	m_isQuitting = true;

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
	g_theGame->RenderUI();
	g_theRenderer->EndCamera();

	g_theConsole->Render( *g_theRenderer );
	// debug render
	DebugRenderScreenTo( g_gameCamera->GetColorTarget() );
	DebugRenderWorldToCamera( g_gameCamera );
}

void App::EndFrame()
{
	g_theGame->EndFrame();
	g_theCameraSystem->EndFrame();
	g_theInputSystem->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudioSystem->EndFrame();
	g_theConsole->EndFrame();
	DebugRenderEndFrame();
}






