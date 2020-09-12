#include "Vec4.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec2.hpp"


Vec4 Vec4::ZERO	= Vec4( 0.f, 0.f, 0.f, 0.f );
Vec4 Vec4::ONE	= Vec4( 1.f, 1.f, 1.f, 1.f );


Vec4::Vec4( float inputX, float inputY, float inputZ, float inputW )
{
	x = inputX;
	y = inputY;
	z = inputZ;
	w = inputW;
}

Vec4::Vec4( Vec3 XYZ, float w )
	:x(XYZ.x)
	,y(XYZ.y)
	,z(XYZ.z)
	,w(w)

{
}

Vec4::Vec4( Vec2 XY )
	:x(XY.x)
	,y(XY.y)
{
	z = 0;
	w = 0;
}

void Vec4::SetFormText( const char* text )
{
	Strings dimensions = SplitStringOnDelimiter( text, "," );
	std::string dimensionX = GetStringWithoutSpace( dimensions[0].c_str() );
	std::string dimensionY = GetStringWithoutSpace( dimensions[1].c_str() );
	std::string dimensionZ = GetStringWithoutSpace( dimensions[2].c_str() );
	std::string dimensionW = GetStringWithoutSpace( dimensions[3].c_str() );

	x = (float)atof( dimensionX.c_str() );
	y = (float)atof( dimensionY.c_str() );
	z = (float)atof( dimensionZ.c_str() );
	w = (float)atof( dimensionW.c_str() );

}

void Vec4::SetXYZ( Vec3 color )
{
	x = color.x;
	y = color.y;
	z = color.z;

}

void Vec4::SetW( float intensity )
{
	w = intensity;
}

const Vec4 Vec4::operator-( const Vec4& vecToSubtract ) const
{
	return Vec4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}

const Vec4 Vec4::operator+( const Vec4& vecToAdd ) const
{
	return Vec4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}

const Vec4 Vec4::operator*=( const float uniformScale ) const
{
	return Vec4( uniformScale*x, uniformScale*y, uniformScale*z, uniformScale*w );
}
