#pragma once
#include <queue>
#include "Engine/Platform/Window.hpp"
#include "Engine/Input/XboxController.hpp"
#include "Engine/Input/MouseController.hpp"
#include "Engine/Input/KeyBoardController.hpp"

constexpr int MAX_XBOX_CONTROLLERS = 4;

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
	void Update();

	// Mouse 
	IntVec2 GetMouseRawDesktopPos() const; // interface to know
<<<<<<< HEAD
	Vec2 GetNormalizedMousePos() const; // within opengl windows
	Vec2 GetNormalizedMousePosInClient( void* hWnd ) const; // within opengl windows
	Vec2 GetMouseRawPosInClient( void* hWnd ) const;
	Vec2 GetNormalizedMousePosInCamera( void* hWnd, const Camera& camera ) const;
	
	void UpdateMouseButtonState( MouseButtonID mouseID, bool isPressed );
	void UpdateMouseWheelAmount( float deltaAmount );
	void ResetMouseWheel();
	float GetMouseWheelAmount() const;
	bool IsMouseButtonDown( MouseButtonID buttonID ) const;
	bool WasMouseButtonJustPressed( MouseButtonID buttonID ) const;
	bool WasMouseButtonJustReleased( MouseButtonID buttonID ) const;

	Vec2 GetRelativeMovementPerFrame() const { return m_relativeMovement; }
	void UpdateRelativeMode();
	void HideSystemCursor();
	void ShowShstemCursor();
	void ClipSystemCursor();
	void SetCursorMode( CursorMode mode );
	
	// Xbox controller
	void UpdateKeyBoardButton( unsigned char inValue, bool isPressed );
	const KeyBoardController& GetKeyBoardController(){return m_keyBoardController; }
	const XboxController& GetXboxController(int controllerID);

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

	CursorMode m_cursorMode = CURSOR_ABSOLUTE;

	Window* m_topWindow;
	std::queue<char> m_characters;
	Vec2 m_relativeMovement;
};