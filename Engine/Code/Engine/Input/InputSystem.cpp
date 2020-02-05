#include <windows.h>
#include "InputSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Input/KeyBoardController.hpp"
#include "Engine/input/MouseController.hpp"

extern HWND g_hWnd;

void InputSystem::Startup()
{
	m_mouseController		= new MouseController();
	m_keyBoardController	= new KeyBoardController();
	m_mouseController->InitialButtonStates();
	m_keyBoardController->InitialButtonStates();

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
}

void InputSystem::Shutdown()
{
	delete m_keyBoardController;
	delete m_mouseController;
	for( int cIndex = 0; cIndex < MAX_XBOX_CONTROLLERS; cIndex++ ) {
		delete m_controllers[cIndex];
		m_controllers[cIndex] = nullptr;
	}
	m_keyBoardController	= nullptr;
	m_mouseController		= nullptr;
}

IntVec2 InputSystem::GetMouseRawDesktopPos() const
{
	POINT rawmouseDesktopPos;
	GetCursorPos( &rawmouseDesktopPos );
	return IntVec2( rawmouseDesktopPos.x, rawmouseDesktopPos.y);
}

Vec2 InputSystem::GetNormalizedMousePos() const
{
	POINT screenMousePos;
	GetCursorPos( &screenMousePos );
	ScreenToClient( g_hWnd, &screenMousePos );
	Vec2 mouseClientPos( (float) screenMousePos.x, (float) screenMousePos.y );

	RECT clientRect;
	GetClientRect( g_hWnd, &clientRect );
	AABB2 clientBounds( (float)clientRect.left, (float)clientRect.bottom, (float)clientRect.right, (float)clientRect.top ); //need to check the contrustor
	Vec2 mouseNormalizedPos = clientBounds.GetUVForPoint( mouseClientPos );
	mouseNormalizedPos.x = ClampZeroToOne( mouseNormalizedPos.x );
	mouseNormalizedPos.y = ClampZeroToOne( mouseNormalizedPos.y );
	return mouseNormalizedPos;	
}


Vec2 InputSystem::GetNormalizedMousePosInCamera( const Camera& camera ) const
{
	Vec2 mouseNormalizedPos = GetNormalizedMousePos();
	AABB2 orthoBounds( camera.GetOrthoBottomLeft(), camera.GetOrthoTopRight() );
	Vec2 mousePosInCamera = orthoBounds.GetPointAtUV( mouseNormalizedPos );
	return mousePosInCamera;
}

void InputSystem::UpdateMouseButtonState( bool leftDown, bool middleDown, bool rightDown )
{
	m_mouseController->UpdateButtonCurrentFrame( MOUSE_BUTTON_LEFT, leftDown );
	m_mouseController->UpdateButtonCurrentFrame( MOUSE_BUTTON_MIDDLE, middleDown );
	m_mouseController->UpdateButtonCurrentFrame( MOUSE_BUTTON_RIGHT, rightDown );
}

void InputSystem::UpdateMouseWheelAmount( float deltaAmount )
{
	m_mouseController->UpdateWheelThisFrame( deltaAmount );
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

bool InputSystem::WasMouseButtonJustReleased( MouseButtonID buttonID ) const
{
	const KeyButtonState& tempMouseButtonState = m_mouseController->m_mouseButton[buttonID];
	return tempMouseButtonState.WasJustReleased();
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

