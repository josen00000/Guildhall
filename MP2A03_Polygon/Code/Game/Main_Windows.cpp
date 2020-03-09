#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include<Game/App.hpp>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Platform/Window.hpp"

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove all OpenGL references out of Main_Win32.cpp once you have a RenderContext
// Both of the following lines should be relocated to the top of Engine/Renderer/RenderContext.cpp
//


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this useful macro to a more central place, e.g. Engine/Core/EngineCommon.hpp
//
#define UNUSED(x) (void)(x);


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move these constants to Game/GameCommon.hpp or elsewhere
// 


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move each of these items to its proper place, once that place is established
// Initialize
//
extern App* g_theApp;
extern InputSystem* g_theInputSystem;
Window* g_theWindow = nullptr;

bool g_isQuitting = false;							// ...becomes App::m_isQuitting
													// HWND g_hWnd = nullptr;								// ...becomes WindowContext::m_windowHandle
													// HDC g_displayDeviceContext = nullptr;				// ...becomes WindowContext::m_displayContext
													// HGLRC g_openGLRenderingContext = nullptr;			// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "Windows OpenGL Test App";	// ...becomes ??? (Change this per project!)


													//-----------------------------------------------------------------------------------------------
													// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
													// This function is called by Windows whenever we ask it for notifications
													//
													// #SD1ToDo: We will move this function to a more appropriate place later on...
													//
LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	switch( wmMessageCode )
	{

		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{

		g_theApp->HandleQuitRequested();
		return 0; // "Consumes" this message (tells Windows "okay, we handled it")
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		unsigned char asKey = (unsigned char)wParam;
		if( asKey==VK_ESCAPE ) {
			g_theApp->HandleQuitRequested();
		}
		g_theInputSystem->UpdateKeyBoardButton( asKey, true );
		break;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		unsigned char asKey = (unsigned char)wParam;
		g_theInputSystem->UpdateKeyBoardButton( asKey, false );
		//			#SD1ToDo: Tell the App and InputSystem about this key-released event...
		break;
	}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


void RunMessagePump()
{
	MSG queuedMessage;
	for( ;; )
	{
		const BOOL wasMessagePresent = PeekMessage( &queuedMessage, NULL, 0, 0, PM_REMOVE );
		if( !wasMessagePresent )
		{
			break;
		}

		TranslateMessage( &queuedMessage );
		DispatchMessage( &queuedMessage ); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}






//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE, _In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );
	UNUSED( applicationInstanceHandle );


	g_theApp = new App();
	g_theWindow = new Window();
	g_theWindow->Open( APP_NAME, CLIENT_ASPECT, 0.9f );
	g_theApp->Startup();
	while( !g_theApp->IsQuitting() ) {
		//Sleep(16); testing
		RunMessagePump();
		g_theApp->RunFrame();



	}

	// shut down and destroy the app

	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp = nullptr;

	g_theWindow->Close();
	delete g_theWindow;
	g_theWindow = nullptr;
}


