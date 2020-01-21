#include "FloatRange.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

FloatRange::FloatRange( float minAndMax )
	:minimum(minAndMax)
	,maximum(minAndMax)
{
}

FloatRange::FloatRange( float min, float max )
	:minimum(min)
	,maximum(max)
{
}

FloatRange::FloatRange( const char* asText )
{
	bool result = SetFromText(asText);
	if( !result ){
		ERROR_AND_DIE("Float range constructor in wrong format.");
	}
}

bool FloatRange::IsFloatInRange( float value ) const
{
	if( value <= minimum || value >= maximum){
		return false;
	}
	else{
		return true;
	}
}

bool FloatRange::DoesOverlap( const FloatRange& otherRange ) const
{
	if( IsFloatInRange(otherRange.minimum) || IsFloatInRange(otherRange.maximum) ){
		return true;
	}
	else if( otherRange.IsFloatInRange( minimum ) || otherRange.IsFloatInRange( maximum ) ){
		return true;
	}
	else{
		return false;
	}
}

std::string FloatRange::GetAsString() const
{
	std::string result;
	if( minimum == maximum ){
		result = std::to_string( minimum );
	}
	else{
		result = std::to_string( minimum ) + "~" + std::to_string( maximum );
	}
	return result;
}

float FloatRange::GetRandomInRange( RandomNumberGenerator& rng ) const
{
	return rng.RollRandomFloatInRange( minimum, maximum );
}

void FloatRange::Set( float newMinimum, float newMaximum )
{
	minimum = newMinimum;
	maximum = newMaximum;
}

void FloatRange::AppendFloatRange( float addedFloat )
{
	if( addedFloat > maximum ){
		maximum = addedFloat;
	}
	if( addedFloat < minimum ){
		minimum = addedFloat;
	}
}

bool FloatRange::SetFromText( const char* asText )
{
	std::string tempString = asText;
	Strings result = SplitStringOnDelimiter( tempString, '~' );
	if( result.size() == 1 ){
		minimum = GetFloatFromText( result[0].c_str() );
		maximum = minimum;
	}
	else if( result.size() == 2 ){
		minimum = GetFloatFromText( result[0].c_str() );
		maximum = GetFloatFromText( result[1].c_str() );
	}
	else{
		return false;
	}
	return true;

}
