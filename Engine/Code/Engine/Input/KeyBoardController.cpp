#include"KeyBoardController.hpp"




const KeyButtonState& KeyBoardController::GetButtonState( KeyBoardButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

void KeyBoardController::UpdateButtonCurrentFrame(  unsigned char inputButton, bool isPressed )
{
	m_buttonStates[inputButton].UpdateButtonCurrentFrame(isPressed);

}

void KeyBoardController::UpdateButtonsLastFrame()
{
	for( int buttonIndex = 0; buttonIndex < NUM_KEYBOARD_BUTTONS; buttonIndex++ ){
		m_buttonStates[buttonIndex].UpdateButtonLastFrame();
	}
}

void KeyBoardController::InitialButtonStates()
{
	for( int buttonIndex = 0; buttonIndex < NUM_KEYBOARD_BUTTONS; buttonIndex++ ){
		m_buttonStates[buttonIndex] = KeyButtonState();
	}
}

