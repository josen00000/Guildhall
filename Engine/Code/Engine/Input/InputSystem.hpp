#pragma once
#include "Engine/Input/XboxController.hpp"

constexpr int MAX_XBOX_CONTROLLERS = 4;

class Camera;
class MouseController;
class KeyBoardController;

class InputSystem {
	/*
	 *Engine input system; 
	 *owns 4 XboxController instances (and, later, mouse & keyboard).
	*/
public:
	InputSystem(){}
	~InputSystem(){}

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	// Mouse 
	IntVec2 GetMouseRawDesktopPos() const; // interface to know
	Vec2 GetNormalizedMousePos() const; // within opengl windows
	Vec2 GetNormalizedMousePosInCamera( const Camera& camera ) const;
	
	void UpdateKeyBoardButton(unsigned char inValue, bool isPressed);
	const KeyBoardController* GetKeyBoardController(){ return m_keyBoardController; }
	const XboxController* GetXboxController(int controllerID);

	// keyboard
	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

private:
	XboxController* m_controllers[MAX_XBOX_CONTROLLERS] = {
		new XboxController( 0 ),
		new XboxController( 1 ),
		new XboxController( 2 ),
		new XboxController( 3 )
	};
	KeyBoardController* m_keyBoardController	= nullptr;
	MouseController* m_mouseController			= nullptr;
};