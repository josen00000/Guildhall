#pragma once
#include "Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <Math.h>

float Vec2::CrossProduct( Vec2 const& a, Vec2 const& b )
{
	return a.x * b.y - a.y * b.x;
}

Vec2::Vec2( Vec2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


Vec2::Vec2( const float i_x, const float i_y ) 
	:x ( i_x )
	,y ( i_y )
{

}

Vec2::Vec2( const float i_xy )
	:x ( i_xy )
	,y ( i_xy )
{

}


Vec2::Vec2( IntVec2 const& intVec )
	:x(intVec.x)
	,y(intVec.y)
{

}

Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length/* =1.f */ ) 
{
	return Vec2(( length * cos( directionRadians )), length * sin( directionRadians));
}

Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length/* =1.f */ ) 
{
	float directionRadians = ConvertDegreesToRadians( directionDegrees );
	return Vec2::MakeFromPolarRadians( directionRadians, length );
}

float Vec2::GetLength() const
{
	return sqrt(( x * x ) + ( y * y ));
}

float Vec2::GetSquaredLength() const
{
	return ( x * x ) + ( y * y );
}

float Vec2::GetAngleDegrees() const
{
	float rads = atan2((double)y, (double)x );
	return ConvertRadiansToDegrees( rads );
}

float Vec2::GetAngleRadians() const
{
	return atan2((double)y, (double)x );
}

const Vec2 Vec2::GetNormalized() const
{
	float rads = GetAngleRadians();
	return Vec2( cosf( rads ), sinf( rads ));
}

void Vec2::SetRadians( float i_rads )
{
	float length = GetLength();
	x = length * cos( (double)i_rads );
	y = length * sin( (double)i_rads );
}

void Vec2::SetDegrees( float i_degs )
{
	float rads = ConvertDegreesToRadians( i_degs );
	SetRadians( rads );
}

void Vec2::SetPolarRadians( float i_rads, float i_length )
{
	x = i_length * cos( (double)i_rads );
	y = i_length * sin( (double)i_rads );
}

void Vec2::SetPolarDegrees( float i_degs, float i_length )
{
	float rads = ConvertDegreesToRadians( i_degs );
	SetPolarRadians( rads, i_length );
}

void Vec2::Rotate90Degrees()
{
	float tempX = x;
	x = -y;
	y = x;
}

void Vec2::RotateMinus90Degrees()
{
	float tempX = x;
	x = y;
	y = -x;
}

void Vec2::RotateRadians( float deltaRadians )
{
	float rads = GetAngleRadians();
	rads += deltaRadians;
	SetRadians( rads );
}

void Vec2::RotateDegrees( float deltaDegrees )
{
	float deltaRads = ConvertDegreesToRadians( deltaDegrees );
	RotateRadians( deltaRads );
}

void Vec2::SetLength( float i_length )
{
	float rads = atan2( y , x );
	x = i_length * cos( rads );
	y = i_length * sin( rads );
}

void Vec2::ClampLength( float maxLength, float minLength /*= 0 */ )
{
	float length = GetLength();
	float newLength = ClampFloat( length, maxLength, minLength  );
	float temp = newLength / length	;
	x *= temp;
	y *= temp;
}

void Vec2::Normalize()
{
	float rads = atan2( y, x );
	x = cos( rads );
	y = sin( rads );
}

const Vec2 Vec2::operator+( const Vec2& vecToAdd ) const
{
	return Vec2( x + vecToAdd.x, y + vecToAdd.y );
}

const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const Vec2 Vec2::operator-() const
{
	return Vec2( -x, -y );
}

const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x * uniformScale, y * uniformScale );
}

const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x * vecToMultiply.x, y * vecToMultiply.y );
}

const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x / inverseScale, y / inverseScale );
}

void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}

void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

bool Vec2::operator==( const Vec2& compare ) const
{
	return x == compare.x && y == compare.y; 
}

bool Vec2::operator!=( const Vec2& compare ) const
{
	return !(*this == compare);
}

