#include "Window.hpp"
#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <windows.h>
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

static wchar_t const* WND_CLASS_NAME = TEXT("Simple Window Class");
extern DevConsole* g_theConsole;
extern Window* g_theWindow;

//-----------------------------------------------------------------------------------------------
// Handles Windows (Win32) messages/events; i.e. the OS is trying to tell us something happened.
// This function is called by Windows whenever we ask it for notifications
//
// #SD1ToDo: We will move this function to a more appropriate place later on...
//
static LRESULT CALLBACK WindowsMessageHandlingProcedure( HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam )
{
	Window* window = (Window*) ::GetWindowLongPtr( windowHandle, GWLP_USERDATA );
// 	if( wmMessageCode == WM_KEYDOWN ){
// 		int a = 0;
// 	}
// 	if( wmMessageCode == WM_KEYUP ) {
// 		int b = 0;
// 	}
	switch( wmMessageCode )
	{

		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
		case WM_CLOSE:
		{
			// TODO SD2 quit the windows
			//g_theApp->HandleQuitRequested();
			g_theConsole->ExecuteCommand( "quit" );
			window->m_isWindowClose = true;
			return 0; // "Consumes" this message (tells Windows "okay, we handled it")
		}

		case WM_ACTIVATE:{
			if( window == nullptr ){ break; }
			InputSystem* input = window->GetInputSystem();

			switch( wParam )
			{
				case WA_ACTIVE: {
					input->ClipSystemCursor();
					break;
				}
				case WA_INACTIVE: {
					input->UnClipSystemCursor();
					break;
				}
				default:
					break;
			}
			break;
		}


		// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
		case WM_KEYDOWN:
		{
			InputSystem* input = window->GetInputSystem();
			unsigned char asKey = (unsigned char)wParam;
			input->UpdateKeyBoardButton( asKey, true );
			break;
		}

		// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
		case WM_KEYUP:
		{
			unsigned char asKey = (unsigned char)wParam;
			InputSystem* input = window->GetInputSystem();
			input->UpdateKeyBoardButton( asKey, false );
			break;
		}
		case WM_CHAR: {
			wchar_t character = (wchar_t)wParam;
			if( 32 <= character && character <= 126 ) {
				InputSystem* input = window->GetInputSystem();
				input->PushCharacter( (char)character );
			}
			if( character == 0x03 ){
				g_theConsole->SendSelectedStringToClipBoard();
			}
			if( character == 0x16 ) {
				g_theConsole->ReceiveStringFromClipBoard();
			}
			break;

		}
	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc( windowHandle, wmMessageCode, wParam, lParam );
}


static void RegisterWindowClass(){
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset( &windowClassDescription, 0, sizeof( windowClassDescription ) );
	windowClassDescription.cbSize = sizeof( windowClassDescription );
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle( NULL );
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = WND_CLASS_NAME;
	RegisterClassEx( &windowClassDescription );
}


static void UnregisterWindowClass(){
	::UnregisterClass( TEXT( "Simple Window Class"), GetModuleHandle( NULL ));
}


Window::Window()
{
	RegisterWindowClass();
}

Window::~Window()
{
	UnregisterWindowClass();
}


bool Window::Open( std::string const& title, float clientAspect, float maxClientFractionOfDesktop ){
	const DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED; // SD2 TODO: style for full screen
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect( desktopWindowHandle, &desktopRect );
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	
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
	MultiByteToWideChar( GetACP(), 0, title.c_str(), -1, windowTitle, sizeof( windowTitle ) / sizeof( windowTitle[0] ) );
	
	HWND hwnd = CreateWindowEx(
		windowStyleExFlags,
		WND_CLASS_NAME,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE) ::GetModuleHandle( NULL ),
		NULL );

	if( hwnd == nullptr ) { return false; }


	ShowWindow( hwnd, SW_SHOW );
	SetForegroundWindow( hwnd );
	SetFocus( hwnd );

	//g_displayDeviceContext = GetDC( hwnd );

	HCURSOR cursor = LoadCursor( NULL, IDC_ARROW );
	SetCursor( cursor );
	
	m_hwnd = (void*)hwnd;
	m_clientWidth = (int) clientWidth;
	m_clientHeight = (int) clientHeight;
	::SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR)this );
	return true;
}

void Window::Close(){
	HWND hwnd = (HWND)m_hwnd;
	if( NULL == hwnd ) {
		return ;
	}
	::DestroyWindow( hwnd );
	m_hwnd = nullptr;
}

void Window::BeginFrame()
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

int Window::GetClientWidth() const
{
	return m_clientWidth;
}

int Window::GetClientHeight() const
{
	return m_clientHeight;
}

Vec2 Window::GetClientCenter() const
{
	RECT clientRect;
	GetClientRect( static_cast<HWND>(m_hwnd), &clientRect );
	return Vec2( (float)clientRect.right / 2, (float)clientRect.bottom / 2 );
}

void Window::SetInputSystem( InputSystem* input )
{
	m_inputSystem = input;
}

void* Window::GetTopWindowHandle()
{
	// TODO function should get the current top window and return. 
	// TODO Debug
	// temp just return g_theWindow. there is only one windows using. 
	// Need to implement later.
	HWND topWindowHandle;
	topWindowHandle = GetTopWindow( NULL );
	return (void*)g_theWindow->m_hwnd;
	//return (void*)topWindowHandle; 
}