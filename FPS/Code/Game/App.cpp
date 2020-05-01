#include<stdio.h>
#include "App.hpp"
#include <iostream>
#include <windows.h>
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/Time/Time.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/DebugRender.hpp"


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


#include <vector>
#include <functional>

void SubscribeTest( float t ) {
	DebuggerPrintf( "test in subscribe %f", t );
}

void AddTest( int a, int b ) {
	int c = a + b;
	DebuggerPrintf( "result %i = %i + %i" , a, b, c );
}

class TestClass {
public:
	TestClass( int v)
		:m_value(v){}

	void SomeMethod( int a, int b ) {
		int c = a + b;
		DebuggerPrintf( "method %i = %i + %i", c, a, b );
	}
private:
	int m_value;
};


template<typename ...ARGS>
class Delegate {
public:
public:
	using function_t = std::function<void(ARGS...)>;
	using c_callback_t = void(*)(ARGS...);
	
	struct sub_t {
		void const* func_id;
		void const* obj_id;
		function_t callable;

		inline bool operator==( sub_t const& other ) const { return func_id == other.func_id;}
	};


	void subscribe( c_callback_t const& cb ) {
		sub_t sub;
		sub.func_id = &cb;
		sub.callable = cb;
		subscribe(sub);
	}
	void invoke( ARGS const &... args ) {
		for( sub_t& cb : m_callbacks ) {
			cb.callable( args... );
		}
	}
	void unsubscribe( c_callback_t const& cb ) {
		sub_t sub;
		sub.func_id = cb;
		unsubscribe( sub );
	}
	template <typename OBJ_TYPE>
	void subscribe_method( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(ARGS...) ) {
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *( void const**)&mcb;
		sub.callable = [=]( ARGS ...args) { (obj->*mcb)( args...); };

		subscribe( sub );
	}

	template <typename OBJ_TYPE>
	void unsubscribe_method( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(ARGS...) ) {
		sub_t sub;
		sub.obj_id = obj;
		sub.func_id = *( void const**) &mcb;

		unsubscribe( sub );
	}

// 	template< typename OBJ_TYPE>
// 	void subscribe_method( OBJ_TYPE* obj, void(OBJ_TYPE::* mcb)(ARGS...) ) {
// 
// 	}

	void operator() ( ARGS const&... args ) {
		invoke( args... );
	}

private:
	void subscribe( sub_t const& sub ) {
		m_callbacks.push_back( sub );
	}
	void unsubscribe( sub_t const& sub ) {
		for( uint i = 0; i < m_callbacks.size(); ++i ) {
			if( m_callbacks[i] == sub ) {
				m_callbacks.erase( m_callbacks.begin() + i );
				return;
			}
		}
	}
	std::vector<sub_t> m_callbacks;
};



void App::Startup()
{
	TemplateTesting();
	StringTesting();
	g_theRenderer		= new RenderContext();
	g_theInputSystem	= new InputSystem();
	g_theEventSystem	= new EventSystem();

	Clock::SystemStartUp();
	
	g_theWindow->SetInputSystem( g_theInputSystem );
	g_theRenderer->StartUp( g_theWindow );
	g_theInputSystem->Startup();

	g_camera			= Camera::CreatePerspectiveCamera( 60, -0.1f, -100.f );
	g_camera->m_debugString = "gamecamera";
	g_camera->SetClearMode( CLEAR_NONE, Rgba8::DARK_GRAY );
	g_camera->EnableClearColor( Rgba8::DARK_GRAY );
	g_camera->EnableClearDepth( 1 );
	g_camera->SetRenderContext( g_theRenderer );
	g_UICamera			= new Camera( "uicamera", Vec2( UI_CAMERA_MIN_X, UI_CAMERA_MIN_Y ), Vec2( UI_CAMERA_MAX_X, UI_CAMERA_MAX_Y ) );
	g_devCamera			= new Camera( "devCamera", Vec2( -32, -18 ), Vec2( 32, 18 ) );
	g_theGame			= new Game( g_camera, g_UICamera );

	g_theGame->Startup();
	EnableDebugRendering();
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

void App::TemplateTesting()
{
	Delegate<float> update;
	Delegate<int, int> add;

	TestClass obj( 7 );


	update.subscribe( SubscribeTest );
	add.subscribe( AddTest );
	add.subscribe_method( &obj, &TestClass::SomeMethod );

	update.unsubscribe( SubscribeTest );

	update.invoke( 3.15f );
	add.invoke( 4, 6 );
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

void App::Shutdown()
{
	g_theGame->Shutdown();
	g_theInputSystem->Shutdown();
	g_theConsole->Shutdown();
	g_theRenderer->Shutdown();

	Clock::SystemShutDown();
	
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
	Clock::BeginFrame();
	g_theInputSystem->BeginFrame();
	g_theRenderer->BeginFrame();
}

void App::Update( float deltaSeconds )
{
	//g_theRenderer->UpdateFrameTime( deltaSeconds );
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

