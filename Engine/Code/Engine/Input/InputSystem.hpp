#pragma once
#include <queue>
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/MouseController.hpp"
#include "Engine/Input/KeyBoardController.hpp"

constexpr int MAX_XBOX_CONTROLLERS = 4;

class Camera;

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
	void UpdateMouseButtonState( MouseButtonID mouseID, bool isPressed );
	void UpdateMouseWheelAmount( float deltaAmount );
	void ResetMouseWheel();
	float GetMouseWheelAmount() const;
	bool IsMouseButtonDown( MouseButtonID buttonID ) const;
	bool WasMouseButtonJustPressed( MouseButtonID buttonID ) const;
	bool WasMouseButtonJustReleased( MouseButtonID buttonID ) const;

	// Xbox controller
	const XboxController* GetXboxController(int controllerID);

	// keyboard
	void UpdateKeyBoardButton(unsigned char inValue, bool isPressed);
	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;
	const KeyBoardController* GetKeyBoardController(){ return m_keyBoardController; }
	void PushCharacter( char character );
	bool PopCharacter( char* out );
	void ClearCharacters();

private:
	XboxController* m_controllers[MAX_XBOX_CONTROLLERS] = {
		new XboxController( 0 ),
		new XboxController( 1 ),
		new XboxController( 2 ),
		new XboxController( 3 )
	};
	KeyBoardController* m_keyBoardController	= nullptr;
	MouseController* m_mouseController			= nullptr;
	std::queue<char> m_characters;
};