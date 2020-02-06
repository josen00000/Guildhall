#include "MouseController.hpp"

void MouseController::UpdateButtonCurrentFrame( unsigned char inputButton, bool isPressed )
{
	m_mouseButton[inputButton].UpdateButtonCurrentFrame( isPressed );
}

void MouseController::UpdateButtonsLastFrame()
{
	for( int buttonIndex = 0; buttonIndex < NUM_MOUSE_BUTTONS; buttonIndex++ ){
		m_mouseButton[buttonIndex].UpdateButtonLastFrame();
	}
}

void MouseController::InitialButtonStates()
{
	for( int buttonIndex = 0; buttonIndex < NUM_MOUSE_BUTTONS; buttonIndex++ ) {
		m_mouseButton[buttonIndex] = KeyButtonState();
	}
	m_scrollAmount = 0;
}

void MouseController::UpdateWheelThisFrame( float deltaAmount )
{
	m_scrollAmount += deltaAmount;
}

void MouseController::ResetMouseWheel()
{
	m_scrollAmount = 0;
}
