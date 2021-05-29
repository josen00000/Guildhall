#include <math.h>
#include <cmath>
#include <sstream>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include <sstream>

#define _USE_MATH_DEFINES 
#ifndef M_PI
#define  M_PI  3.1415926f 

#endif


//Static objects
Vec2 Vec2::ZERO						= Vec2( 0.f, 0.f );
Vec2 Vec2::ONE						= Vec2( 1.f, 1.f );
Vec2 Vec2::ZERO_ONE					= Vec2( 0.f, 1.f );
Vec2 Vec2::ONE_ZERO					= Vec2( 1.f, 0.f );
Vec2 Vec2::UP_NORMAL_DIRECTION		= Vec2( 0.f, 1.f );
Vec2 Vec2::DOWN_NORMAL_DIRECTION	= Vec2( 0.f, -1.f );
Vec2 Vec2::LEFT_NORMAL_DIRECTION	= Vec2( -1.f, 0.f );
Vec2 Vec2::RIGHT_NORMAL_DIRECTION	= Vec2( 1.f, 0.f );

//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}
Vec2::Vec2( const Vec3& copy )
	:x(copy.x)
	,y(copy.y)
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const float initialX, const float initialY )
	: x( initialX )
	, y( initialY )
{
}


Vec2::Vec2( const IntVec2& copyFrom )
{
	x=(float)copyFrom.x;
	y=(float)copyFrom.y;
}

Vec2::Vec2( float XY )
{
	x = XY;
	y = XY;
}

const Vec2 Vec2::MakeFromPolarRadians( float directionRadians, float length )
{
	const Vec2 tem_Vec2=Vec2(length*cos(directionRadians),length*sin(directionRadians));
	return tem_Vec2;
}

const Vec2 Vec2::MakeFromPolarDegrees( float directionDegrees, float length )
{
	const float tem_Radians = ConvertDegreesToRadians(directionDegrees);
	const Vec2 tem_Vec2= Vec2(length*cos(tem_Radians),length*sin(tem_Radians));
	return tem_Vec2;
}

float Vec2::GetLength() const
{
	return sqrtf(x*x+y*y);
}

float Vec2::GetSquaredLength() const
{
	return (x*x+y*y);
}

float Vec2::GetAngleRadians() const
{
	return (atan2(y,x));
}

float Vec2::GetAngleDegrees() const
{
	const float tem_Degrees = ConvertRadiansToDegrees( GetAngleRadians() );
	return tem_Degrees;
}

const Vec2 Vec2::GetRotated90Degrees() const
{	
	return GetRotatedRadians(M_PI/2);
}

const Vec2 Vec2::GetRotatedMinus90Degrees() const
{
	return GetRotatedRadians(-M_PI/2);
}

const Vec2 Vec2::GetRotatedRadians( float deltaRadians ) const
{
	const float tem_Radius=GetLength();
	float tem_Radians=GetAngleRadians();
	tem_Radians+=deltaRadians;
	const float tem_x=tem_Radius*cos( tem_Radians );
	const float tem_y=tem_Radius*sin( tem_Radians );
	return  Vec2( tem_x, tem_y );

}

const Vec2 Vec2::GetRotatedDegrees( float deltaDegrees ) const
{
	const float tem_Radius=GetLength();
	const float tem_DeltaRadians=ConvertDegreesToRadians(deltaDegrees);
	float tem_Radians=	GetAngleRadians();
	tem_Radians+=tem_DeltaRadians;
	float const tem_x=tem_Radius*cos( tem_Radians );
	float const tem_y=tem_Radius*sin( tem_Radians );
	return Vec2( tem_x, tem_y );

}

const Vec2 Vec2::GetClamped( float maxLength ) const
{
	Vec2 tem_Vec2;
	const float tem_Radius=GetLength();
	if(tem_Radius>maxLength){
		const float tem_Radian=GetAngleRadians();
		tem_Vec2=Vec2(maxLength*cos(tem_Radian),maxLength*sin(tem_Radian));
		return tem_Vec2; 
	}
	else{
		tem_Vec2=Vec2(*this);
		return tem_Vec2;
	}
}

const Vec2 Vec2::GetNormalized() const
{
	const float tem_Radians=GetAngleRadians();
	return Vec2(cos(tem_Radians),sin(tem_Radians));
}

const Vec2 Vec2::GetReflected( const Vec2& normal ) const
{
	Vec2 normalProjectedVec2 = GetProjectedOnto2D(*this, normal);
	Vec2 vec2T = *this - normalProjectedVec2;
	Vec2 result = vec2T - normalProjectedVec2 ;
	return result;
}

std::string Vec2::ToDebugString()
{	
	return Stringf( "Vec2 X:%.2f, Y:%.2f", x, y );
	//return std::string( "Vec2 x: " + std::to_string( x ) + ".  y: " + std::to_string( y ) );
}

std::string Vec2::ToString()
{
	return std::string( std::to_string( x ) + "," + std::to_string( y ) );
}

float Vec2::NormalizeAndGetPreviousLength()
{
	const float tem_Radius=GetLength();
	Normalize();
	return tem_Radius;
	
	
}

void Vec2::Reflect( const Vec2& normal )
{
	Vec2 temVec2 = this->GetReflected(normal);
	x = temVec2.x;
	y = temVec2.y;
}

float Vec2::CrossProduct( Vec2 a, Vec2 b )
{
	return a.x * b.y - a.y * b.x;
}

// Mutators(non-const methods)
void Vec2::SetAngleRadians( float newAngleRadians ) {
	const float tem_Len=GetLength();
	x=tem_Len*cos(newAngleRadians);
	y=tem_Len*sin(newAngleRadians);
	
		
}
void Vec2::SetAngleDegrees(float newAngleDegess){
	const float tem_Len = GetLength();
	const float tem_Radians = ConvertDegreesToRadians(newAngleDegess);
	x=tem_Len*cos( tem_Radians );
	y=tem_Len*sin( tem_Radians );
}

 
void Vec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	x=newLength*cos(newAngleRadians);
	y=newLength*sin(newAngleRadians);
}

void Vec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	const float tem_Radians=ConvertDegreesToRadians(newAngleDegrees);
	x=newLength*cos( tem_Radians );
	y=newLength*sin( tem_Radians );
}

void Vec2::SetFromText( const char* text )
{
	Strings dimensions = SplitStringOnDelimiter(text, ",");
	std::string dimensionX = GetStringWithoutSpace(dimensions[0].c_str());
	std::string dimensionY = GetStringWithoutSpace(dimensions[1].c_str());
	x = (float)atof( dimensionX.c_str() );
	y = (float)atof( dimensionY.c_str() );

}

void Vec2::Rotate90Degrees()
{
	RotateRadians(M_PI/2);
	
	
}

void Vec2::RotateMinus90Degrees()
{
	RotateRadians(-M_PI/2);
}


void Vec2::RotateRadians( float deltaRadians )
{
	const float tem_Radius=GetLength();
	float tem_Radians=GetAngleRadians();
	tem_Radians+=deltaRadians;
	x=tem_Radius*cos(tem_Radians);
	y=tem_Radius*sin(tem_Radians);
}

void Vec2::RotateDegrees( float deltaDegrees )
{
	
	const float tem_DeltaRadians=ConvertDegreesToRadians(deltaDegrees);
	RotateRadians(tem_DeltaRadians);
}

void Vec2::SetLength(  float newLength )
{
	float tem_Radians=GetAngleRadians();
	x=newLength*cos(tem_Radians);
	y=newLength*sin(tem_Radians);
}


void Vec2::ClampLength( float maxLength )
{
	float tem_Radius=GetLength();
	if(tem_Radius>maxLength){
		const float tem_Radians=atan2(y,x);
		x=maxLength*cos(tem_Radians);
		y=maxLength*sin(tem_Radians);
	}
}

void Vec2::Normalize()
{
	const float tem_Radians=atan2(y,x);
	x=cos( tem_Radians );
	y=sin( tem_Radians );
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator+ ( const Vec2& vecToAdd ) const
{
	return Vec2( x+vecToAdd.x, y+vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2( x-vecToSubtract.x, y-vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2( -x,-y );
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2( x*uniformScale, y*uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2( x*vecToMultiply.x, y*vecToMultiply.y );
}



//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2( x/inverseScale, y/inverseScale );
}



//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x-=vecToSubtract.x;
	y-=vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x*=uniformScale;
	y*=uniformScale;

}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x/=uniformDivisor;
	y/=uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x=copyFrom.x;
	y=copyFrom.y;
	
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2( uniformScale*vecToScale.x, uniformScale*vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	if(x==compare.x&&y==compare.y){
		return true;
	}
	else{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	if(x!=compare.x||y!=compare.y){
		return true;
	}
	else{
		return false;
	}
}


