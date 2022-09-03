#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include <cmath>

float ConvertRadiansToDegrees( const float rads )
{
	return rads / M_PI * 180.f;
}

float ConvertDegreesToRadians( const float degs )
{
	return degs	/ 180.f * M_PI;
}

float ClampFloat( float input, float max, float min )
{
	if( input >= max ) 
	{
		return max;
	}
	else if( input <= min )
	{
		return min;
	}
	else 
	{
		return input;
	}
}

float RangeMapFloat( float inStart, float inEnd, float outStart, float outEnd, float inValue )
{
	float inDisp = inValue - inStart;
	float fraction = inDisp / ( inEnd - inStart );
	return outStart + ( outEnd - outStart ) * fraction;
}

bool IsVec2MostEqual( Vec2 const& a, Vec2 const& b, float epsilon /*= 0.01f */ )
{
	return ( isFloatMostEqual( a.x, b.x, epsilon ) && isFloatMostEqual( a.y, b.y, epsilon ));
}

bool isFloatMostEqual( float a, float b, float epsilon /*= 0.01f */ )
{
	float diff = b - a;
	diff = abs( diff );
	return diff < epsilon;
}
