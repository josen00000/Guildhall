#include<engine/Input/XboxController.hpp>
#include<Windows.h>
#include<Xinput.h>
#pragma comment( lib, "xinput9_1_0" ) 
#include<Engine/Math/MathUtils.hpp>
XboxController::XboxController( int controllerID )
	:m_controllerID(controllerID)
{
	
	InitialButtonStates();
}

XboxController::~XboxController()
{

}

const KeyButtonState& XboxController::GetButtonState( XboxButtonID buttonID ) const
{
	return m_buttonStates[buttonID];
}

void XboxController::Update()
{
	//
	XINPUT_STATE xboxControllerState;
	memset(&xboxControllerState,0,sizeof(xboxControllerState));
	DWORD errorStatus = XInputGetState(m_controllerID,&xboxControllerState);
	if(errorStatus == ERROR_SUCCESS){
		m_isConnected = true;
		XINPUT_GAMEPAD& gamepad=xboxControllerState.Gamepad;
		UpdateJoystick(m_leftJoystick,gamepad.sThumbLX,gamepad.sThumbLY);
		UpdateJoystick(m_rightJoystick,gamepad.sThumbRX,gamepad.sThumbRY);
		UpdateTrigger(m_leftTriggerValue,gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerValue,gamepad.bRightTrigger);
		UpdateButton(XBOX_BUTTON_ID_A,gamepad.wButtons,XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B,gamepad.wButtons,XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X,gamepad.wButtons,XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y,gamepad.wButtons,XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_ID_BACK,gamepad.wButtons,XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START,gamepad.wButtons,XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_ID_LSHOULDER,gamepad.wButtons,XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RSHOULDER,gamepad.wButtons,XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_LTHUMB,gamepad.wButtons,XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RTHUMB,gamepad.wButtons,XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT,gamepad.wButtons,XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP,gamepad.wButtons,XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_DPAD_LEFT,gamepad.wButtons,XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_DOWN,gamepad.wButtons,XINPUT_GAMEPAD_DPAD_DOWN);
	
	}
	else if(errorStatus==ERROR_DEVICE_NOT_CONNECTED){
	
		Reset();
		
	}

}

void XboxController::Reset()
{
	m_isConnected=false;
	for( int buttonIndex=0; buttonIndex<NUM_XBOX_BUTTONS; buttonIndex++ ) {
		m_buttonStates[buttonIndex].Reset();
	}
	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	m_leftTriggerValue=0.f;
	m_rightTriggerValue=0.f;

}

void XboxController::UpdateTrigger( float& triggerValue, unsigned char rawValue )
{
	// update the trigger value
	float tem = (float)rawValue;
	triggerValue=RangeMapFloat(0.f,255.f,0.f,1.f,tem);
}

void XboxController::UpdateJoystick( AnalogJoystick& joystick, short rawX, short rawY )
{
	//update the Joystick value
	joystick.UpdatePosition(rawX,rawY);
}

void XboxController::UpdateButton( XboxButtonID buttonID, unsigned short buttonFlags, unsigned short buttonFlag )
{
	if((buttonFlag&buttonFlags)==buttonFlag){
		m_buttonStates[buttonID].UpdateStatus(true);
	}
	else{
		m_buttonStates[buttonID].UpdateStatus(false);
	}
}

void XboxController::InitialButtonStates()
{
	for (int buttonIndex=0;buttonIndex<NUM_XBOX_BUTTONS;buttonIndex++){
		m_buttonStates[buttonIndex]= KeyButtonState();
	}

}

void XboxController::setVibration(float value)const 
{
	
	XINPUT_VIBRATION vibrationInfo;
	memset( &vibrationInfo, 0, sizeof( vibrationInfo ) );
	float tem=(float)value*65535;
	vibrationInfo.wLeftMotorSpeed=(unsigned char)tem;
	vibrationInfo.wLeftMotorSpeed=(unsigned char)tem;


}

