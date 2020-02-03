#pragma once
class KeyButtonState {
public:
	KeyButtonState(){}
	~KeyButtonState(){}
public:
	void UpdateStatus( bool isNowPressed );
	void UpdateButtonCurrentFrame( bool isNowPressed );
	void UpdateButtonLastFrame();
	bool IsPressed() const  { return m_isPressed; }
	bool WasJustPressed() const;
	bool WasJustReleased() const;
	void Reset();

private :
	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;

};