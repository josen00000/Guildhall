#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>			// #include this (massive, platform-specific) header in very few places
#include <math.h>
#include <cassert>
#include <crtdbg.h>
#include<Game/App.hpp>


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Remove all OpenGL references out of Main_Win32.cpp once you have a RenderContext
// Both of the following lines should be relocated to the top of Engine/Renderer/RenderContext.cpp
//
#include <gl/gl.h>					// Include basic OpenGL constants and function declarations
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library


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
bool g_isQuitting = false;							// ...becomes App::m_isQuitting
HWND g_hWnd = nullptr;								// ...becomes WindowContext::m_windowHandle
HDC g_displayDeviceContext = nullptr;				// ...becomes WindowContext::m_displayContext
HGLRC g_openGLRenderingContext = nullptr;			// ...becomes RenderContext::m_apiRenderingContext
const char* APP_NAME = "LD45 App";	// ...becomes ??? (Change this per project!)
//extern  g_theInputSystem;

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
			//g_theInputSystem->UpdateKeyBoardButton(asKey, true);
			g_theApp->HandleKeyPressed(asKey);
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char) wParam;
			//g_theInputSystem->UpdateKeyBoardButton(asKey, false);
			g_theApp->HandleKeyReleased(asKey);
//			#SD1ToDo: Tell the App and InputSystem about this key-released event...
			break;
		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
void CreateOSWindow( void* applicationInstanceHandle, float clientAspect )
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT( "Simple Window Class" );
	RegisterClassEx( &windowClassDescription );

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 0.90f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if( clientAspect > desktopAspect )
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * clientAspect;
	}

	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx( &windowRect, windowStyleFlags, FALSE, windowStyleExFlags );

	WCHAR windowTitle[1024];
	MultiByteToWideChar( GetACP(), 0, APP_NAME, -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );
	g_hWnd = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE) applicationInstanceHandle,
		NULL );

	ShowWindow( g_hWnd, SW_SHOW );
	SetForegroundWindow( g_hWnd );
	SetFocus( g_hWnd );

	g_displayDeviceContext = GetDC( g_hWnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
}


//------------------------------------------------------------------------------------------------
// Given an existing OS Window, create a Rendering Context (RC) for, say, OpenGL to draw to it.
//
void CreateRenderContext()
{
	// Creates an OpenGL rendering context (RC) and binds it to the current window's device context (DC)
	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	memset( &pixelFormatDescriptor, 0, sizeof( pixelFormatDescriptor ) );
	pixelFormatDescriptor.nSize = sizeof( pixelFormatDescriptor );
	pixelFormatDescriptor.nVersion = 1;
	pixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
	pixelFormatDescriptor.cColorBits = 24;
	pixelFormatDescriptor.cDepthBits = 24;
	pixelFormatDescriptor.cAccumBits = 0;
	pixelFormatDescriptor.cStencilBits = 8;

	int pixelFormatCode = ChoosePixelFormat( g_displayDeviceContext, &pixelFormatDescriptor );
	SetPixelFormat( g_displayDeviceContext, pixelFormatCode, &pixelFormatDescriptor );
	g_openGLRenderingContext = wglCreateContext( g_displayDeviceContext );
	wglMakeCurrent( g_displayDeviceContext, g_openGLRenderingContext );

	// #SD1ToDo: move all OpenGL functions (including those below) to RenderContext.cpp (only!)
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
}


void DrawFilledAABB2(float minX,float minY,float maxX,float maxY,unsigned char red, unsigned char green, unsigned char blue, unsigned char alpha){
	glBegin( GL_TRIANGLES );
	{
		// First triangle (3 vertexes, each preceded by a color)
		glColor4ub( red, green, blue, alpha );
		glVertex3f( minX,minY,0.f) ;
		glVertex3f( maxX,minY,0.f) ;
		glVertex3f( maxX,maxY,0.f) ;

		glVertex3f( minX,minY,0.f) ;
		glVertex3f( maxX,maxY,0.f) ;
		glVertex3f( minX,maxY,0.f) ;


		
	}
	glEnd();
}
//-----------------------------------------------------------------------------------------------
// Processes all Windows messages (WM_xxx) for this app that have queued up since last frame.
// For each message in the queue, our WindowsMessageHandlingProcedure (or "WinProc") function
//	is called, telling us what happened (key up/down, minimized/restored, gained/lost focus, etc.)
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
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
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Startup()
//
/*
void TheApp_Startup( void* applicationInstanceHandle, const char* commandLineString )
{
	UNUSED( commandLineString );
	CreateOSWindow( applicationInstanceHandle, CLIENT_ASPECT );	// #SD1ToDo: replace these two lines...
	CreateRenderContext();										// #SD1ToDo: ...with the two lines below
}
*/


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Shutdown()
//
/*
void TheApp_Shutdown()
{
}
*/

//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::BeginFrame()
//

void TheApp_BeginFrame()
{
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Update()
//
void TheApp_Update()
{
}


//-----------------------------------------------------------------------------------------------
// Some simple OpenGL example drawing code.
// This is the graphical equivalent of printing "Hello, world."
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::Render()
// #SD1ToDo: Move all OpenGL code to RenderContext.cpp (only)
//
void TheApp_Render()
{
	// Establish a 2D (orthographic) drawing coordinate system: (0,0) bottom-left to (10,10) top-right
	// #SD1ToDo: This will be replaced by a call to g_renderer->BeginView( m_worldView ); or similar
	glLoadIdentity();
	glOrtho( 0.f, 10.f, 0.f, 10.f, 0.f, 1.f );

	// Clear all screen (backbuffer) pixels to medium-blue
	// #SD1ToDo: This will become g_renderer->ClearColor( Rgba8( 0, 0, 127, 255 ) );
	glClearColor( 0.f, 0.f, 0.5f, 1.f ); // Note; glClearColor takes colors as floats in [0,1], not bytes in [0,255]
	glClear( GL_COLOR_BUFFER_BIT ); // ALWAYS clear the screen at the top of each frame's Render()!

	// Draw some triangles (provide 3 vertexes each)
	// #SD1ToDo: Move all OpenGL code into RenderContext.cpp (only); call g_renderer->DrawVertexArray() instead
	glBegin( GL_TRIANGLES );
	{
		// First triangle (3 vertexes, each preceded by a color)
		glColor4ub( 255, 0, 0, 255 );
		glVertex3f( 1.f, 1.f,0.f );

		glColor4ub( 0, 255, 0, 255 );
		glVertex3f( 8.f, 2.f,0.f );

		glColor4ub( 255, 255, 255, 255 );
		glVertex3f( 3.f, 8.f,0.f );

		// Second triangle (3 vertexes, each preceded by a color)
		glColor4ub( 100, 0, 0, 255 );
		glVertex2f( 3.f, 5.f );

		glColor4ub( 100, 0, 0, 255 );
		glVertex2f( 5.f, 3.f );

		glColor4ub( 100, 0, 0, 255 );
		glVertex2f( 6.f, 7.f );
	}
	glEnd();
	DrawFilledAABB2(2.f,2.f,8.f,4.f,255,0,0,255);
}


//-----------------------------------------------------------------------------------------------
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::EndFrame()
//
void TheApp_EndFrame()
{
	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame
}


//-----------------------------------------------------------------------------------------------
// One "frame" of the game.  Generally: Input, Update, Render.  We call this 60+ times per second.
// #SD1ToDo: Move this function to Game/App.cpp and rename it to  TheApp::RunFrame()
//
void RunFrame()
{
	RunMessagePump();

	TheApp_BeginFrame();	// #SD1ToDo: ...becomes g_theApp->BeginFrame();
	TheApp_Update();		// #SD1ToDo: ...becomes g_theApp->Update();
	TheApp_Render();		// #SD1ToDo: ...becomes g_theApp->Render();
	TheApp_EndFrame();		// #SD1ToDo: ...becomes g_theApp->EndFrame();
}


//-----------------------------------------------------------------------------------------------
int WINAPI WinMain( _In_ HINSTANCE applicationInstanceHandle, _In_opt_ HINSTANCE,_In_ LPSTR commandLineString, _In_ int )
{
	UNUSED( commandLineString );
	UNUSED(applicationInstanceHandle);

	CreateOSWindow(applicationInstanceHandle,CLIENT_ASPECT);
	CreateRenderContext();

	g_theApp=new App();
	g_theApp->Startup();
	while(!g_theApp->IsQuitting()){
		Sleep(16);
		RunMessagePump();
		g_theApp->RunFrame();
		SwapBuffers(g_displayDeviceContext);

	
	}

	// shut down and destroy the app

	g_theApp->Shutdown();
	delete g_theApp;
	g_theApp=nullptr;

	/*
	

	// Program main loop; keep running frames until it's time to quit
	while( !g_isQuitting )			// #SD1ToDo: ...becomes:  !g_theApp->IsQuitting()
	{
		RunFrame();					// #SD1ToDo: ...becomes g_theApp->RunFrame()
	}

	TheApp_Shutdown();				// #SD1ToDo: ...becomes:  g_theApp->Shutdown();  AND  delete g_theApp;  AND  g_theApp = nullptr;
	*/
	
}


