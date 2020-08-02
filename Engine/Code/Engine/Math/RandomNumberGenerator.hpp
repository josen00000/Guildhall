#pragma once
#include <cstdint>
#include <stdlib.h>
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"

//constexpr long MAX_RAND_NOT_INCLUSIVE = UINT32_MAX + 1;  

class RandomNumberGenerator {
public:
	RandomNumberGenerator() = default;
	~RandomNumberGenerator(){}
	explicit RandomNumberGenerator( int seed );
	//constexpr unsigned int Get1dNoiseUint( int positionX, unsigned int seed);
	int		RollRandomIntLessThan( int maxNotInclusive);
	int		RollRandomIntInRange( int minInclusive, int maxInclusive );
	float	RollRandomFloatLessThan( float maxNotInclusive );
	float	RollRandomFloatInRange( float minInclusive, float maxInclusive );
	float	RollRandomFloatZeroToOneInclusive();
	float	RollRandomFloatZeroToAlmostOne();
	bool	RollPercentChance( float probabilityOfReturningTrue );
	int		Rand();
	Vec2	RollRandomVec2InRange(const Vec2& minInclusive, const Vec2& maxInclusive );
	Vec3	RollRandomVec3InRange(const Vec3& minInclusive, const Vec3& maxInclusive );


public:
	int m_seed = 0;
	int m_position = 0;
};