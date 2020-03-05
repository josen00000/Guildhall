#include "InputSystem.hpp"
#include <windows.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Input/KeyBoardController.hpp"
#include "Engine/input/MouseController.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/Camera.hpp"


void InputSystem::Startup()
{
	m_mouseController		= new MouseController();
	m_keyBoardController	= new KeyBoardController();
	m_mouseController->InitialButtonStates();
	m_keyBoardController->InitialButtonStates();
	ClipSystemCursor();

}

void InputSystem::BeginFrame()
{
	for (int controllerIndex=0;controllerIndex<MAX_XBOX_CONTROLLERS;controllerIndex++){
		m_controllers[controllerIndex]->Update();
	}
}

void InputSystem::EndFrame()
{
	m_keyBoardController->UpdateButtonsLastFrame();
	m_mouseController->UpdateButtonsLastFrame();
}

void InputSystem::Shutdown()
{
	UnClipSystemCursor();
	ShowSystemCursor();
	delete m_keyBoardController;
	delete m_mouseController;
	for( int cIndex = 0; cIndex < MAX_XBOX_CONTROLLERS; cIndex++ ) {
		delete m_controllers[cIndex];
		m_controllers[cIndex] = nullptr;
	}
	m_keyBoardController	= nullptr;
	m_mouseController		= nullptr;
}

void InputSystem::Update()
{
	switch( m_cursorMode )
	{
	case CURSOR_ABSOLUTE:
		m_relativeMovement = Vec2::ZERO;
		return;
	case CURSOR_RELATIVE:
		UpdateRelativeMode();
		break;
	default:
		break;
	}
}

IntVec2 InputSystem::GetMouseRawDesktopPos() const
{
	POINT rawmouseDesktopPos;
	GetCursorPos( &rawmouseDesktopPos );
	return IntVec2( rawmouseDesktopPos.x, rawmouseDesktopPos.y);
}

Vec2 InputSystem::GetNormalizedMousePosInClient( void* hWnd ) const
{
	HWND hWnd1 = (HWND)hWnd;
	POINT screenMousePos;
	GetCursorPos( &screenMousePos );
	ScreenToClient( hWnd1, &screenMousePos );
	Vec2 mouseClientPos( (float) screenMousePos.x, (float) screenMousePos.y );
	
	RECT clientRect;
	GetClientRect( hWnd1, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.bottom, (float)clientRect.right, (float)clientRect.top  ); 
	Vec2 mouseNormalizedPos = clientBounds.GetUVForPoint( mouseClientPos );
	mouseNormalizedPos.x = ClampZeroToOne( mouseNormalizedPos.x );
	mouseNormalizedPos.y = ClampZeroToOne( mouseNormalizedPos.y );
	return mouseNormalizedPos;	
}


Vec2 InputSystem::GetMouseRawPosInClient( void* hWnd ) const
{
	HWND hWnd1 = (HWND)hWnd;
	POINT screenMousePos;
	GetCursorPos( &screenMousePos );
	ScreenToClient( hWnd1, &screenMousePos );
	Vec2 mouseClientPos( (float)screenMousePos.x, (float)screenMousePos.y );
	return mouseClientPos;
}

Vec2 InputSystem::GetNormalizedMousePosInCamera( void* hWnd, const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = GetNormalizedMousePosInClient( hWnd );
	AABB2 orthoBounds( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
	Vec2 mousePosInCamera = orthoBounds.GetPointAtUV( mouseNormalizedPos );
	return mousePosInCamera;
}

void InputSystem::UpdateMouseButtonState( MouseButtonID mouseID, bool isPressed )
{
	m_mouseController->UpdateButtonCurrentFrame( (unsigned char)mouseID, isPressed );
}

void InputSystem::UpdateMouseWheelAmount( float deltaAmount )
{
	m_mouseController->UpdateWheelThisFrame( deltaAmount );
}

void InputSystem::ResetMouseWheel()
{
	m_mouseController->ResetMouseWheel();
}

float InputSystem::GetMouseWheelAmount() const
{
	return m_mouseController->m_scrollAmount;
}

bool InputSystem::IsMouseButtonDown( MouseButtonID buttonID ) const
{
	const KeyButtonState& tempMouseButtonState = m_mouseController->m_mouseButton[buttonID];
	return tempMouseButtonState.IsPressed();
}

void InputSystem::UpdateKeyBoardButton(unsigned char inValue, bool isPressed)
{
	m_keyBoardController->UpdateButtonCurrentFrame(inValue,isPressed);
}

bool InputSystem::WasMouseButtonJustPressed( MouseButtonID buttonID ) const
{
	const KeyButtonState& tempMouseButtonState = m_mouseController->m_mouseButton[buttonID];
	return tempMouseButtonState.WasJustPressed();
}

bool InputSystem::WasMouseButtonJustReleased( MouseButtonID buttonID ) const{
	const KeyButtonState& tempMouseButtonState = m_mouseController->m_mouseButton[buttonID];
	return tempMouseButtonState.WasJustReleased();
}
void InputSystem::HideSystemCursor()
{
	while( ShowCursor( false ) > 0 ){}
}

void InputSystem::ShowSystemCursor()
{
	while( ShowCursor( true ) < 0 ){}
}

void InputSystem::ClipSystemCursor()
{
	HWND topHWnd = (HWND)Window::GetTopWindowHandle();
	RECT winRect;
	GetWindowRect( topHWnd, &winRect );
	// get windows rect
	ClipCursor( &winRect );
}

void InputSystem::UnClipSystemCursor()
{
	ClipCursor( NULL );
}

void InputSystem::SetCursorMode( CursorMode mode )
{
	switch( mode )
	{
	case CURSOR_ABSOLUTE:
		ShowSystemCursor();
		break;
	case CURSOR_RELATIVE:
		HideSystemCursor();
		break;
	default:
		break;
	}
	m_cursorMode = mode;
}

void InputSystem::UpdateRelativeMode( )
{
	HWND hwnd = (HWND)Window::GetTopWindowHandle();
	//move mouse to center of screen
	// calculate the delta of the cursor
	// get top window
	//POINT cursorPos;
	
	IntVec2 positionThisFrame = GetMouseRawDesktopPos();
	static IntVec2 positionLastFrame = positionThisFrame; 
	IntVec2 rawRelativeMovement = positionThisFrame - positionLastFrame;

	// temp GetClientCenter in this
	// Need to debug and get client center in windows
	RECT clientRect;
	GetClientRect( hwnd, &clientRect );
	m_relativeMovement = Vec2 ( ( (float)rawRelativeMovement.x / clientRect.right ), ( (float)rawRelativeMovement.y / clientRect.bottom ) );

	POINT windowCenterPoint;
	windowCenterPoint.x = clientRect.right / 2;
	windowCenterPoint.y = clientRect.bottom / 2; 
	ClientToScreen( hwnd, &windowCenterPoint );
	SetCursorPos( windowCenterPoint.x, windowCenterPoint.y );
	//GetCursorPos( &cursorPos );
	IntVec2 windowCenter = IntVec2( windowCenterPoint.x, windowCenterPoint.y );
	positionLastFrame = windowCenter;
}

const XboxController* InputSystem::GetXboxController( int controllerID )
{
	return m_controllers[controllerID];
}

bool InputSystem::IsKeyDown( unsigned char keyCode ) const
{
	const KeyButtonState& temButtonState = m_keyBoardController->m_buttonStates[keyCode];
	return temButtonState.IsPressed();
}

bool InputSystem::WasKeyJustPressed( unsigned char keyCode ) const
{
	const KeyButtonState& temButtonState = m_keyBoardController->m_buttonStates[keyCode];
	return temButtonState.WasJustPressed();
}

bool InputSystem::WasKeyJustReleased( unsigned char keyCode ) const
{
	const KeyButtonState& temButtonState = m_keyBoardController->m_buttonStates[keyCode];
	return temButtonState.WasJustReleased();
}

void InputSystem::PushCharacter( char character )
{
	m_characters.push( character );	
}

bool InputSystem::PopCharacter( char* out )
{
	if( m_characters.empty() ){
		return false;
	}
	else {
		*out = m_characters.front();
		m_characters.pop();
		return true;
	}
}

void InputSystem::ClearCharacters()
{
	while( !m_characters.empty() ) {
		m_characters.pop();
	}
}


		