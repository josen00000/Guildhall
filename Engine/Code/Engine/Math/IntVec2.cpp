#include <math.h>
#include <cmath>
#include "IntVec2.hpp"
#include "Engine/Core/StringUtils.hpp"
#include<Engine/Math/Vec3.hpp>
#include<Engine/Math/vec2.hpp>
#include<Engine/Math/MathUtils.hpp>

#define _USE_MATH_DEFINES 
#ifndef M_PI
#define  M_PI  3.1415926f 

#endif

const IntVec2 IntVec2::ZERO = IntVec2( 0, 0 );
const IntVec2 IntVec2::ONE = IntVec2( 1, 1 );

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2( const IntVec2& copy )
	: x( copy.x )
	, y( copy.y )
{
}



IntVec2::IntVec2( const int initialX, const int initialY )
	: x( initialX )
	, y( initialY )
{

}

IntVec2::IntVec2( const Vec2 vec2 )
{
	x=(int)vec2.x;
	y=(int)vec2.y;
}

IntVec2::IntVec2( int XY )
{
	x = XY;
	y = XY;
}

float IntVec2::GetLength() const
{
	return sqrtf((float)(x*x+y*y));
}

float IntVec2::GetLengthSquared() const
{
	return (float)(x*x+y*y);
}

int IntVec2::GetTaxicabLength() const
{
	return abs(x)+abs(y);

}

float IntVec2::GetAngleRadians() const
{
	return (float)(atan2(y,x));
}

float IntVec2::GetAngleDegrees() const
{
	const float tem_Degrees=ConvertRadiansToDegrees(GetAngleRadians());
	return tem_Degrees;
}



const IntVec2 IntVec2::GetRotated90Degrees() const
{	

	int temX=-y;
	int temY=x;
	IntVec2 result=IntVec2( temX, temY );
	return result;
}

const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	int temX=y;
	int temY=-x;
	IntVec2 result=IntVec2(temX,temY);
	return result;
}


void IntVec2::Rotate90Degrees()
{
	int temX=-y;
	int temY=x;
	x=temX;
	y=temY;

}

void IntVec2::RotateMinus90Degrees()
{
	int temX=y;
	int temY=-x;
	x=temX;
	y=temY;

}

void IntVec2::SetFromText( const char* text )
{
	Strings dimensions = SplitStringOnDelimiter( text, "," );
	std::string dimensionX = GetStringWithoutSpace( dimensions[0].c_str() );
	std::string dimensionY = GetStringWithoutSpace( dimensions[1].c_str() );
	x = atoi( dimensionX.c_str() );
	y = atoi( dimensionY.c_str() );
}

/*
const IntVec2 IntVec2::GetClamped( float maxLength ) const
{
	IntVec2 tem_Vec2;
	const float tem_Radius=GetLength();
	if(tem_Radius>maxLength){
		const float tem_Radian=GetAngleRadians();
		tem_Vec2=IntVec2((int)(maxLength*cos(tem_Radian)),(int)(maxLength*sin(tem_Radian)));
		return tem_Vec2; 
	}
	else{
		tem_Vec2=IntVec2(*this);
		return tem_Vec2;
	}
}

const IntVec2 IntVec2::GetNormalized() const
{
	const float tem_Radians=GetAngleRadians();
	return IntVec2( (int)(cos( tem_Radians )), sin( tem_Radians ) );
}*/
/*

float IntVec2::NormalizeAndGetPreviousLength()
{
	const float tem_Radius=GetLength();
	Normalize();
	return tem_Radius;


}
*/
// Mutators(non-const methods)
/*

void IntVec2::SetAngleRadians( float newAngleRadians ) {
	const float tem_Len=GetLength();
	x=tem_Len*cos(newAngleRadians);
	y=tem_Len*sin(newAngleRadians);


}
void IntVec2::SetAngleDegrees(float newAngleDegess){
	const float tem_Len=GetLength();
	const float tem_Radians=ConvertDegreesToRadians(newAngleDegess);
	x=tem_Len*cos( tem_Radians );
	y=tem_Len*sin( tem_Radians );
}


void IntVec2::SetPolarRadians( float newAngleRadians, float newLength )
{
	x=newLength*cos(newAngleRadians);
	y=newLength*sin(newAngleRadians);
}

void IntVec2::SetPolarDegrees( float newAngleDegrees, float newLength )
{
	const float tem_Radians=ConvertDegreesToRadians(newAngleDegrees);
	x=newLength*cos( tem_Radians );
	y=newLength*sin( tem_Radians );
}




void IntVec2::RotateDegrees( float deltaDegrees )
{

	const float tem_DeltaRadians=ConvertDegreesToRadians(deltaDegrees);
	RotateRadians(tem_DeltaRadians);
}

void IntVec2::SetLength(  float newLength )
{
	float tem_Radians=GetAngleRadians();
	x=newLength*cos(tem_Radians);
	y=newLength*sin(tem_Radians);
}


void IntVec2::ClampLength( float maxLength )
{
	float tem_Radius=GetLength();
	if(tem_Radius>maxLength){
		const float tem_Radians=atan2(y,x);
		x=maxLength*cos(tem_Radians);
		y=maxLength*sin(tem_Radians);
	}
}

void IntVec2::Normalize()
{
	const float tem_Radians=atan2(y,x);
	x=cos( tem_Radians );
	y=sin( tem_Radians );
}

*/
//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator+ ( const IntVec2& vecToAdd ) const
{
	return IntVec2( x+vecToAdd.x, y+vecToAdd.y );
}


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator-( const IntVec2& vecToSubtract ) const
{
	return IntVec2( x-vecToSubtract.x, y-vecToSubtract.y );
}


//------------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator-() const
{
	return IntVec2( -x,-y );
}


//-----------------------------------------------------------------------------------------------

const IntVec2 IntVec2::operator*( int uniformScale ) const
{
	return IntVec2( x*uniformScale, y*uniformScale );
}


//------------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator*( const IntVec2& vecToMultiply ) const
{
	return IntVec2( x*vecToMultiply.x, y*vecToMultiply.y );
}




//-----------------------------------------------------------------------------------------------
/*
const IntVec2 IntVec2::operator/( float inverseScale ) const
{
	return IntVec2( x/inverseScale, y/inverseScale );
}*/



//-----------------------------------------------------------------------------------------------
void IntVec2::operator+=( const IntVec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator-=( const IntVec2& vecToSubtract )
{
	x-=vecToSubtract.x;
	y-=vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator*=( const int uniformScale )
{
	x*=uniformScale;
	y*=uniformScale;

}


/*
//-----------------------------------------------------------------------------------------------
void IntVec2::operator/=( const float uniformDivisor )
{
	x/=uniformDivisor;
	y/=uniformDivisor;
}*/


//-----------------------------------------------------------------------------------------------
void IntVec2::operator=( const IntVec2& copyFrom )
{
	x=copyFrom.x;
	y=copyFrom.y;

}


//-----------------------------------------------------------------------------------------------

const IntVec2 operator*( int uniformScale, const IntVec2& vecToScale )
{
	return IntVec2( uniformScale*vecToScale.x, uniformScale*vecToScale.y );
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==( const IntVec2& compare ) const
{
	if(x==compare.x&&y==compare.y){
		return true;
	}
	else{
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=( const IntVec2& compare ) const
{
	if(x!=compare.x||y!=compare.y){
		return true;
	}
	else{
		return false;
	}
}


