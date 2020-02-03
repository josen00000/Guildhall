#include "KeyButtonState.hpp"

void KeyButtonState::UpdateStatus( bool isNowPressed )
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = isNowPressed;
}

void KeyButtonState::UpdateButtonCurrentFrame(bool isNowPressed)
{
	m_isPressed = isNowPressed;
}

void KeyButtonState::UpdateButtonLastFrame()
{
	m_wasPressedLastFrame = m_isPressed;
}

bool KeyButtonState::WasJustPressed() const
{
	if( !m_wasPressedLastFrame && m_isPressed ){
		return true;
	}
	else{
		return false;
	}
}

bool KeyButtonState::WasJustReleased() const
{
	if( m_wasPressedLastFrame && !m_isPressed ) {
		return true;
	}
	else {
		return false;
	}
}

void KeyButtonState::Reset()
{
	m_isPressed = false;
	m_wasPressedLastFrame = false;
}

