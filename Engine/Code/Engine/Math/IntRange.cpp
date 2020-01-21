#include "IntRange.hpp"
#include <string>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IntRange::IntRange( int minAndMax )
	:minimum(minAndMax)
	,maximum(minAndMax)
{
}

IntRange::IntRange( int min, int max )
	:minimum(min)
	,maximum(max)
{
}

IntRange::IntRange( const char* asText )
{
	std::string tempString = asText;
	Strings result = SplitStringOnDelimiter( tempString, "~");
	if( result.size() == 1){
		minimum = GetIntFromText( result[0].c_str() );
		maximum = minimum;
	}
	else{
		if( result.size() != 2){
			ERROR_AND_DIE( "the int range constructor got input with wrong dimension");
		}
		minimum = GetIntFromText( result[0].c_str() );
		maximum = GetIntFromText( result[1].c_str() );
	}
}

bool IntRange::IsIntInRange( int value ) const
{
	if( value < minimum || value > maximum){
		return false;
	}
	return true;
}

bool IntRange::DoesOverlap( const IntRange& otherRange ) const
{
	if( IsIntInRange(otherRange.minimum) || IsIntInRange(otherRange.maximum) ){
		return true;
	}
	else if( otherRange.IsIntInRange( minimum ) || otherRange.IsIntInRange( maximum )){
		return true;
	}
	else{
		return false;
	}
}

std::string IntRange::GetAsString() const
{
	if( minimum == maximum ){
		return std::to_string(minimum);
	}
	else{
		return std::to_string(minimum) + "~" + std::to_string(maximum);
	}
}

int IntRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	int result = rng.RollRandomIntInRange( minimum, maximum );
	return result;
}

void IntRange::Set( int newMinimum, int newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;

}

bool IntRange::SetFromText( const char* asText )
{
	std::string tempString = asText;
	Strings result = SplitStringOnDelimiter( tempString, '~' );
	if( result.size() == 1 ) {
		minimum = GetIntFromText( result[0].c_str() );
		maximum = minimum;
		return true;
	}
	else if( result.size() == 2) {
		minimum = GetIntFromText( result[0].c_str() );
		maximum = GetIntFromText( result[1].c_str() );
		return true;
	}
	else{
		return false;
	}
	
}
