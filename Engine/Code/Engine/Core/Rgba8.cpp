#include "Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"


Rgba8 Rgba8::WHITE		= Rgba8( 255, 255, 255 );
Rgba8 Rgba8::BLACK		= Rgba8( 0, 0, 0 );
Rgba8 Rgba8::RED		= Rgba8( 255, 0, 0 );
Rgba8 Rgba8::HALFRED	= Rgba8( 255, 0, 0, 50 );
Rgba8 Rgba8::GREEN		= Rgba8( 0, 255, 0 );
Rgba8 Rgba8::HALFGREEN	= Rgba8( 0, 255, 0, 50 );
Rgba8 Rgba8::BLUE		= Rgba8( 0, 0, 255 );
Rgba8 Rgba8::HALFBLUE	= Rgba8( 0, 0, 255, 50 );
Rgba8 Rgba8::YELLOW		= Rgba8( 255, 255, 0 );
Rgba8 Rgba8::GRAY		= Rgba8( 125, 125, 125, 125 );
Rgba8 Rgba8::DARK_GRAY	= Rgba8( 50, 50, 50 );


Rgba8::Rgba8(unsigned char ini_r, unsigned char ini_g, unsigned char ini_b, unsigned char ini_a)
	:r(ini_r)
	,g(ini_g)
	,b(ini_b)
	,a(ini_a)
{	
}

Rgba8 Rgba8::GetLerpColor( Rgba8 a, Rgba8 b, float value )
{
	Rgba8 result;
	result.r = (unsigned char)(a.r + (b.r - a.r) * value);
	result.g = (unsigned char)(a.g + (b.g - a.g) * value);
	result.b = (unsigned char)(a.b + (b.b - a.b) * value);
	result.a = (unsigned char)(a.a + (b.a - a.a) * value);
	return result;
}

Rgba8 Rgba8::GetColorFromVec4( Vec4 input )
{
	Rgba8 result;
	result.r = (unsigned char)RangeMapFloat( -1.f, 1.f, 0.f, 255.f, input.x );
	result.g = (unsigned char)RangeMapFloat( -1.f, 1.f, 0.f, 255.f, input.y );
	result.b = (unsigned char)RangeMapFloat( -1.f, 1.f, 0.f, 255.f, input.z );
	result.a = (unsigned char)RangeMapFloat( -1.f, 1.f, 0.f, 255.f, input.w );
	return result;
}

Vec3 Rgba8::GetVec3Color() const
{
	Vec3 v3_color;
	v3_color.x = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, r );
	v3_color.y = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, g );
	v3_color.z = RangeMapFloat( 0.f, 255.f, 0.f, 1.f, b );
	return v3_color;
}

void Rgba8::SetFromText( const char* text )
{
	Strings dimensions = SplitStringOnDelimiter( text, "," );
	std::string dimensionR = GetStringWithoutSpace( dimensions[0].c_str() );
	std::string dimensionG = GetStringWithoutSpace( dimensions[1].c_str() );
	std::string dimensionB = GetStringWithoutSpace( dimensions[2].c_str() );
	
	r = (unsigned char)atoi( dimensionR.c_str() );
	g = (unsigned char)atoi( dimensionG.c_str() );
	b = (unsigned char)atoi( dimensionB.c_str() );
	a = 255;
	if(dimensions.size() == 4){
		std::string dimensionA = GetStringWithoutSpace(dimensions[3].c_str());
		a = (unsigned char)atoi( dimensionA.c_str());
	}

}

bool Rgba8::operator==( const Rgba8 compareWith ) const
{
	if( r == compareWith.r && g == compareWith.g && b == compareWith.b && a == compareWith.a ){
		return true;
	}
	else{
		return false;
	}
}
