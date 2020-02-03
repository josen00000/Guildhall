#pragma once

enum MouseButtonID {
	MOUSE_BUTTON_INVALID = -1,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_MIDDLE,
	MOUSE_BUTTON_RIGHT,
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

private:
	KeyButtonState m_mouseButton[NUM_MOUSE_BUTTONS]
};