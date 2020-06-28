#include "RandomNumberGenerator.hpp"
#include "Engine/Math/RawNoise.hpp"
#include "Engine/Math/MathUtils.hpp"

int INTMAX = 2147483647;
int INTMAX_MINUS = -2147483647 - 1;

RandomNumberGenerator::RandomNumberGenerator( int seed )
	:m_seed(seed)
{
	m_position = 0;
}

int RandomNumberGenerator::RollRandomIntLessThan( int maxNotInclusive )
{
	float tempRandomValue = Get1dNoiseZeroToOne(m_position++, m_seed);
	int result = (int)RangeMapFloat(0, 1, 0, (float)maxNotInclusive , tempRandomValue );
	return result;	
}

int RandomNumberGenerator::RollRandomIntInRange( int minInclusive, int maxInclusive )
{
	int tempRandomValue = Get1dNoiseUint( m_position++, m_seed);
	int result = (int)RangeMapFloat( (float)-2147483647 - 1, (float)2147483647, (float)minInclusive, (float)maxInclusive+1 , (float)tempRandomValue );
	return result;
}

float RandomNumberGenerator::RollRandomFloatLessThan( float maxNotInclusive )
{
	float tempRandomValue = Get1dNoiseZeroToOne( m_position++, m_seed );
	float result = tempRandomValue / (float)(1.0000001) * maxNotInclusive;
	return result;
}

/*
 float RandomNumberGenerator::RollRandomFloatInRange( const float minInclusive, const float maxInclusive )
 {
	 float tempRandomValue = Get1dNoiseZeroToOne( m_position++, m_seed );
	 float result = RangeMapFloat( 0, 1, minInclusive, maxInclusive, tempRandomValue );
	 return result;
// 	 float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
// 	 float range=maxInclusive-minInclusive;
// 	 return minInclusive+r*range;
 }
*/

 float RandomNumberGenerator::RollRandomFloatInRange( float minInclusive, float maxInclusive )
 {
	float tempRandomValue = Get1dNoiseZeroToOne( m_position++, m_seed );
	float result = RangeMapFloat( 0.f, 1.f, minInclusive, maxInclusive, tempRandomValue );
	return result;
 }

 float RandomNumberGenerator::RollRandomFloatZeroToOneInclusive()
 {
	float result = Get1dNoiseZeroToOne( m_position++, m_seed );
	return result;
 }

 float RandomNumberGenerator::RollRandomFloatZeroToAlmostOne()
 {
	 float tempRandomValue = Get1dNoiseZeroToOne( m_position++, m_seed );
	 float result = ( tempRandomValue / (float)1.00000001 ) * 1;
	 return result;
 }

 bool RandomNumberGenerator::RollPercentChance( float probabilityOfReturningTrue )
 {
	 //int tempvalue= Rand();
	 float percentChance = Get1dNoiseZeroToOne( m_position++, m_seed );
	 if( probabilityOfReturningTrue >= percentChance){
		return true;
	 }
	 else{
		return false;
	 }
 }

 int RandomNumberGenerator::Rand()
 {
	 //int value = Get1dNoiseZeroToOne( m_position++, m_seed );
	 return 0;
 }

 Vec2 RandomNumberGenerator::RollRandomVec2InRange( const Vec2& minInclusive, const Vec2& maxInclusive )
 {
	 float tempX = RollRandomFloatInRange( minInclusive.x, maxInclusive.x );
	 float tempY = RollRandomFloatInRange( minInclusive.y, maxInclusive.y );
	 return Vec2( tempX, tempY );
 }

 Vec3 RandomNumberGenerator::RollRandomVec3InRange( const Vec3& minInclusive, const Vec3& maxInclusive )
 {
	 float tempX = RollRandomFloatInRange( minInclusive.x, maxInclusive.x );
	 float tempY = RollRandomFloatInRange( minInclusive.y, maxInclusive.y );
	 float tempZ = RollRandomFloatInRange( minInclusive.z, maxInclusive.z );
	 return Vec3( tempX, tempY, tempZ );
 }

