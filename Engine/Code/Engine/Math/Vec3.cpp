#include <math.h>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct Vec2;

// Static objects
Vec3 Vec3::ZERO		= Vec3( 0.f, 0.f, 0.f );
Vec3 Vec3::ONE		= Vec3( 1.f, 1.f, 1.f );
Vec3 Vec3::CENTER	= Vec3( 0.5f, 0.5f, 0.5f );

Vec3::Vec3(const Vec3& copy)
	:x(copy.x),y(copy.y),z(copy.z)
{
}


//-----------------------------------------------------------------------------------------------

Vec3::Vec3( float initialX, float initialY,float initialZ )
	: x( initialX )
	, y( initialY )
	, z(initialZ)
{
}
Vec3::Vec3( const Vec2& copyFrom, float initialZ )
	: x( copyFrom.x )
	, y( copyFrom.y )
	, z( initialZ )
{
}



float Vec3::GetLength() const{
	return sqrtf(x*x+y*y+z*z);
}
float Vec3::GetLengthXY() const{
	return sqrtf(x*x+y*y);
}
float Vec3::GetLengthSquared() const{
	return x*x+y*y+z*z;
}
float Vec3::GetLengthXYSquared() const{
	return x*x+y*y;
}
float Vec3::GetAngleAboutZRadians() const{
	return (atan2f(y,x));
}
float Vec3::GetAngleAboutZDegrees() const{
	const float tem_Degrees=ConvertRadiansToDegrees( GetAngleAboutZRadians() );
	return tem_Degrees;
}
const Vec3 Vec3::GetRotatedAboutZRadians( float deltaRadians )const{
	const float tem_Radius=GetLengthXY();
	float tem_Radians=GetAngleAboutZRadians();
	tem_Radians+=deltaRadians;
	const float tem_x=tem_Radius*cosf( tem_Radians );
	const float tem_y=tem_Radius*sinf( tem_Radians );
	return Vec3( tem_x, tem_y,z );
}
const Vec3 Vec3::GetRotatedAboutZDegrees( float deltaDegrees )const{
	const float tem_Radius=GetLengthXY();
	float tem_Radians=GetAngleAboutZRadians();
	float deltaRadians=ConvertDegreesToRadians(deltaDegrees);
	tem_Radians+=deltaRadians;
	const float tem_x=tem_Radius*cosf( tem_Radians );
	const float tem_y=tem_Radius*sinf( tem_Radians );
	return  Vec3( tem_x, tem_y, z );
}
const Vec3 Vec3::GetClamped( float maxLength )const{
	Vec3 tem_Vec3;
	const float tem_Radius=GetLength();
	if( tem_Radius>maxLength ) {
		
		tem_Vec3= Vec3( x*maxLength/tem_Radius, y*maxLength/tem_Radius,z*maxLength/tem_Radius );
		return tem_Vec3;
	}
	else {
		tem_Vec3= Vec3(*this);
		return tem_Vec3;
	}
}
const Vec3 Vec3::GetNormalized() const{
	Vec3 tem_Vec3;
	const float tem_Radius=GetLength();
	tem_Vec3= Vec3(x/tem_Radius,y/tem_Radius,z/tem_Radius);
	return tem_Vec3;
}

void Vec3::SetText( const char* text )
{
	Strings dimensions = SplitStringOnDelimiter( text, "," );
	std::string dimensionX = GetStringWithoutSpace( dimensions[0].c_str() );
	std::string dimensionY = GetStringWithoutSpace( dimensions[1].c_str() );
	std::string dimensionZ = GetStringWithoutSpace( dimensions[2].c_str() );

	x = (float)atof( dimensionX.c_str() );
	y = (float)atof( dimensionY.c_str() );
	z = (float)atof( dimensionZ.c_str() );

}

const Vec3 Vec3::MakeFromPolarDegrees( float theta, float phi, float length/*=1.f */ )
{
	float cosPhi = CosDegrees( phi );
	Vec3 result = Vec3( ( cosPhi * CosDegrees( theta ) ), SinDegrees( phi ), ( -cosPhi * SinDegrees( theta ) ) );
	result = result * length;
	return result;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator+ ( const Vec3& vecToAdd ) const
{
	return Vec3( x+vecToAdd.x, y+vecToAdd.y,z+vecToAdd.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-( const Vec3& vecToSubtract ) const
{
	return Vec3( x-vecToSubtract.x, y-vecToSubtract.y,z-vecToSubtract.z );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-() const
{
	return Vec3( -x, -y,-z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( float uniformScale ) const
{
	return Vec3( x*uniformScale, y*uniformScale,z*uniformScale );
}


//------------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*( const Vec3& vecToMultiply ) const
{
	return Vec3( x*vecToMultiply.x, y*vecToMultiply.y,z*vecToMultiply.z );
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/( float inverseScale ) const
{
	return Vec3( x/inverseScale, y/inverseScale,z/inverseScale );
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=( const Vec3& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z+= vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=( const Vec3& vecToSubtract )
{
	x-=vecToSubtract.x;
	y-=vecToSubtract.y;
	z-=vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=( const float uniformScale )
{
	x*=uniformScale;
	y*=uniformScale;
	z*=uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=( const float uniformDivisor )
{
	x/=uniformDivisor;
	y/=uniformDivisor;
	z/=uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=( const Vec3& copyFrom )
{
	x=copyFrom.x;
	y=copyFrom.y;
	z=copyFrom.z;

}


//-----------------------------------------------------------------------------------------------



//-----------------------------------------------------------------------------------------------
bool Vec3::operator==( const Vec3& compare ) const
{
	if( x==compare.x&&y==compare.y&&z==compare.z ) {
		return true;
	}
	else {
		return false;
	}
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=( const Vec3& compare ) const
{
	if( x!=compare.x||y!=compare.y||z!=compare.z ) {
		return true;
	}
	else {
		return false;
	}
}


const Vec3 operator*( float uniformScale, const Vec3& vecToScale )
{
	Vec3* tem_Vec3=new Vec3( vecToScale );
	return tem_Vec3->operator*( uniformScale );
}
