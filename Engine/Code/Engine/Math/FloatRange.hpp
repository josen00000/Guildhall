#pragma once
#include <string>

class RandomNumberGenerator;

struct FloatRange
{
public:
	float minimum = 0.f;
	float maximum = 1.f;

public:
	FloatRange() = default;
	explicit FloatRange( float minAndMax );
	explicit FloatRange( float min, float max );
	explicit FloatRange( const char* asText );

	static FloatRange GetIntersectRange( FloatRange a, FloatRange b );

	//Accessors
	bool		IsFloatInRange( float value ) const;
	bool		IsInRange( FloatRange range ) const;
	bool		DoesOverlap( const FloatRange& otherRange ) const;
	std::string	GetAsString() const;
	float		GetRandomInRange( RandomNumberGenerator& rng ) const;

	// Mutators
	void		Set( float newMinimum, float newMaximum );
	void		AppendFloatRange( float addedFloat );
	bool		SetFromText( const char* asText );
};

