#pragma once
#include <queue>
#include "Engine/Input/XboxController.hpp"
#include<Engine/Input/KeyBoardController.hpp>

constexpr int MAX_XBOX_CONTROLLERS=4;

class Camera;

enum CursorMode {
	CURSOR_ABSOLUTE,
	CURSOR_RELATIVE
};

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
	void HideSystemCursor();
	void ShowShstemCursor();
	void ClipSystemCursor();
	void SetCursorMode( CursorMode mode );
	
	void UpdateKeyBoardButton( unsigned char inValue, bool isPressed );
	const KeyBoardController& GetKeyBoardController(){return m_keyBoardController; }
	const XboxController& GetXboxController(int controllerID);

	// keyboard
	bool IsKeyDown( unsigned char keyCode ) const;
	bool WasKeyJustPressed( unsigned char keyCode ) const;
	bool WasKeyJustReleased( unsigned char keyCode ) const;

	void PushCharacter( char character );
	bool PopCharacter( char* out );
	void ClearCharacters();
private:
	XboxController m_controllers[MAX_XBOX_CONTROLLERS]={
		XboxController( 0 ),
		XboxController( 1 ),
		XboxController( 2 ),
		XboxController( 3 )
	};
	KeyBoardController m_keyBoardController;
	std::queue<char> m_characters;
	CursorMode m_cursorMode = CURSOR_ABSOLUTE;
};