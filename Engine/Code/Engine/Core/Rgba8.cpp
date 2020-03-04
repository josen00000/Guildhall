#include "Rgba8.hpp"
#include "Engine/Core/StringUtils.hpp"

Rgba8 Rgba8::WHITE	= Rgba8( 255, 255, 255 );
Rgba8 Rgba8::BLACK	= Rgba8( 0, 0, 0 );
Rgba8 Rgba8::RED	= Rgba8( 255, 0, 0 );
Rgba8 Rgba8::GREEN	= Rgba8( 0, 255, 0 );
Rgba8 Rgba8::BLUE	= Rgba8( 0, 0, 255 );
Rgba8 Rgba8::YELLOW	= Rgba8( 255, 255, 0 );
Rgba8 Rgba8::GRAY = Rgba8( 50, 50, 50);


Rgba8::Rgba8(unsigned char ini_r, unsigned char ini_g, unsigned char ini_b, unsigned char ini_a)
	:r(ini_r)
	,g(ini_g)
	,b(ini_b)
	,a(ini_a)
{	
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
