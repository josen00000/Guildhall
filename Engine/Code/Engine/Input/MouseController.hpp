#pragma once
#include "Engine/Input/KeyButtonState.hpp"

enum MouseButtonID {
	MOUSE_BUTTON_INVALID = -1,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	NUM_MOUSE_BUTTONS
};
class MouseController {
	friend class InputSystem;

public:
	MouseController(){}
	~MouseController(){}

private:
	void UpdateButtonCurrentFrame( unsigned char inputButton, bool isPressed );
	void UpdateButtonsLastFrame();
	void InitialButtonStates();
	void UpdateWheelThisFrame( float deltaAmount );

private:
	KeyButtonState m_mouseButton[NUM_MOUSE_BUTTONS];
	float m_scrollAmount = 0;
};