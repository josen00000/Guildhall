#include<Engine/Input/AnalogJoystick.hpp>
#include<Engine/Math/MathUtils.hpp>
AnalogJoystick::AnalogJoystick( float innerDeadZoneFraction, float outerDeadZoneFraction )
	:m_innerDeadZoneFraction(innerDeadZoneFraction)
	,m_outerDeadZoneFraction(outerDeadZoneFraction)
{
	
}

void AnalogJoystick::UpdatePosition( short rawNormalizedX, short rawNormalizedY )
{

	m_rawPosition.x = RangeMapFloat(-32768.f,32767.f,-1,1,rawNormalizedX);
	m_rawPosition.y = RangeMapFloat(-32768.f,32767.f,-1,1,rawNormalizedY);
	
	CorrectPosition( m_rawPosition );
}

void AnalogJoystick::CorrectPosition( Vec2& rawPosition )
{
	bool isXLessThan0 = false;
	bool isYLessThan0 = false;

	if(rawPosition.x < 0){
		rawPosition.x = -rawPosition.x;
		isXLessThan0 = true;
	}
	if( rawPosition.y < 0 ) {
		rawPosition.y = -rawPosition.y;
		isYLessThan0 = true;
	}
	m_correctedPosition.x = RangeMapFloat(m_innerDeadZoneFraction,m_outerDeadZoneFraction,0.f,1.f,rawPosition.x);
	m_correctedPosition.x = ClampFloat(0.f,1.f,m_correctedPosition.x);
	m_correctedPosition.y = RangeMapFloat( m_innerDeadZoneFraction, m_outerDeadZoneFraction, 0.f, 1.f, rawPosition.y );
	m_correctedPosition.y = ClampFloat( 0.f, 1.f, m_correctedPosition.y );
	
	if(isXLessThan0){
		m_correctedPosition.x=-m_correctedPosition.x;
	}
	if( isYLessThan0 ) {
		m_correctedPosition.y=-m_correctedPosition.y;
	}
	
	//update the degrees and magnitude;
	m_correctedMagnitude=m_correctedPosition.GetLength();
	if(m_correctedMagnitude>0){
		m_correctedDegrees=m_correctedPosition.GetAngleDegrees();
	}
	
	// base on radial
	
	
}

void AnalogJoystick::Reset()
{
	m_rawPosition=Vec2( 0.f, 0.f );
	m_correctedPosition=Vec2( 0.f, 0.f );
	m_correctedDegrees=0.f;
	m_correctedMagnitude=0.f;
}

