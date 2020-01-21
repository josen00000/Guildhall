#pragma once
#include "Engine/Input/KeyButtonState.hpp"
#include "Engine/Input//AnalogJoystick.hpp"
constexpr float INNER_DEADZONE_FRACTION=0.3f;
constexpr float OUTER_DEADZONE_FRACTION=0.95f;
enum XboxButtonID {
	XBOX_BUTTON_ID_INVALID=-1,
	
	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LSHOULDER,
	XBOX_BUTTON_ID_RSHOULDER,
	XBOX_BUTTON_ID_LTHUMB,
	XBOX_BUTTON_ID_RTHUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,

	NUM_XBOX_BUTTONS

};
class XboxController {
	friend class InputSystem;

public:
	explicit XboxController(int controllerID);
	~XboxController();

	bool					isConnected() const			{return m_isConnected;}
	int						GetControllerID() const		{return m_controllerID;}
	const AnalogJoystick&	GetLeftJoystick() const		{return m_leftJoystick;}
	const AnalogJoystick&	GetRightJoystick() const	{return m_rightJoystick;}
	float					GetLeftTrigger() const		{return m_leftTriggerValue;}
	float					GetRightTrigger() const		{return m_rightTriggerValue;}
	const KeyButtonState&	GetButtonState(XboxButtonID buttonID) const;
	void setVibration(float value) const;

private :
	void Update();
	void Reset();
	void UpdateTrigger(float& triggerValue, unsigned char rawValue);
	void UpdateJoystick(AnalogJoystick& joystick, short rawX,short rawY);
	void UpdateButton(XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag);
	void InitialButtonStates();
public:
	const int m_controllerID=-1;
	bool m_isConnected=false;
	KeyButtonState m_buttonStates[NUM_XBOX_BUTTONS];
	AnalogJoystick m_leftJoystick = AnalogJoystick(INNER_DEADZONE_FRACTION,OUTER_DEADZONE_FRACTION);
	AnalogJoystick m_rightJoystick= AnalogJoystick(INNER_DEADZONE_FRACTION,OUTER_DEADZONE_FRACTION);
	float m_leftTriggerValue=0.f;
	float m_rightTriggerValue=0.f;
	
};
