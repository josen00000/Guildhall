#pragma once
#include "IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <math.h>

IntVec2::IntVec2( int i_x, int i_y )
	:x(i_x)
	,y(i_y)
{
}

IntVec2::IntVec2( Vec2 const& vec )
	:x(vec.x)
	,y(vec.y)
{
}

IntVec2::IntVec2( int xy )
	:x(xy)
	,y(xy)
{
}

float IntVec2::GetLength() const
{
	return sqrtf(( x * x ) + ( y * y ));
}

float IntVec2::GetLengthSquared() const
{
	return ( x * x ) + ( y * y );
}

int IntVec2::GetTaxicabLength() const
{
	return abs( x ) +  abs( y );
}

float IntVec2::GetAngleRadians() const
{
	return atan2f( y, x );
}

float IntVec2::GetAngleDegrees() const
{
	float rads = GetAngleRadians();
	return ConvertRadiansToDegrees( rads );
}

const IntVec2 IntVec2::GetRotated90Degrees() const
{
	return IntVec2( -y , x );
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	return IntVec2( y, -x );
}

float IntVec2::CrossProduct( IntVec2 const& a, IntVec2 const& b )
{
	return ( a.x * b.y ) - ( a.y * b.x );
}

const IntVec2 IntVec2::operator+( const IntVec2& vecToAdd ) const
{
	return IntVec2( x + vecToAdd.x, y + vecToAdd.y );
}

const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x - vecToSubtract.x, y - vecToSubtract.y );
}

const IntVec2 IntVec2::operator-() const
{
	return IntVec2( -x, -y );
}

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( x * uniformScale, y * uniformScale );
}

const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	return IntVec2( x * vecToMultiply.x, y * vecToMultiply.y );
}

void IntVec2::operator*=( const int uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

void IntVec2::operator=( const IntVec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

bool IntVec2::operator!=( const IntVec2& compare ) const
{
	return ( x != compare.x ) || ( y != compare.y );
}

bool IntVec2::operator==( const IntVec2& compare ) const
{
	return ( x == compare.x ) && ( y == compare.y );
}
