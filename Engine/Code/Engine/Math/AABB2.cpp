#include "AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

AABB2::AABB2( const Vec2& i_min, const Vec2& i_max )
	:min(i_min)
	,max(i_max)
{

}

AABB2::AABB2( const IntVec2& i_min, const IntVec2& i_max )
	:min(i_min)
	,max(i_max)
{
}

AABB2::AABB2( float minX, float minY, float maxX, float maxY )
{
	min.x = minX;
	min.y = minY;
	max.x = maxX;
	max.y = maxY;
}

AABB2::AABB2( Vec2 const& center, float width, float height )
{
	Vec2 halfSize = Vec2( ( width / 2.f ), ( height / 2.f ));
	min = center - halfSize;
	max = center + halfSize;
}

bool AABB2::IsPointInside( const Vec2& point ) const
{
	if( point.x < min.x ){ return false; }
	if( point.y < min.y ){ return false; }
	if( point.x > max.x ){ return false; }
	if( point.y > max.y ){ return false; }
	return true;
}

bool AABB2::IsPointMostInEdge( const Vec2& point, float epsilon /*= 0.01f */ ) const
{
	if( isFloatMostEqual( point.x, min.x, epsilon ) || isFloatMostEqual( point.x, max.x, epsilon ) ) {
		return ( point.y >= min.y ) && ( point.y <= max.y );
	}
	else if( isFloatMostEqual( point.y, min.y, epsilon ) || isFloatMostEqual( point.y, max.y, epsilon ) ) {
		return ( point.x >= min.x ) && ( point.x <= max.x );
	}
	else {
		 return false;
	}
}

const Vec2 AABB2::GetCenter() const
{
	return Vec2((( min.x + max.x ) / 2.f ), (( min.y + max.y ) / 2.f ) );
}

const Vec2 AABB2::GetDimensions() const
{
	return Vec2(( max.x - min.x ), ( max.y - min.y ));
}

const Vec2 AABB2::GetNearestPoint( const Vec2& refPos ) const
{
	Vec2 nearestPoint = refPos;
	if( refPos.x < min.x ) {
		nearestPoint.x = min.x;
	}
	else if( refPos.x > max.x ) {
		nearestPoint.x = max.x;
	}

	if( refPos.y < min.y ) {
		nearestPoint.y = min.y;
	}
	else if( refPos.y > max.y ) {
		nearestPoint.y = max.y;
	}
	return nearestPoint;
}

const Vec2 AABB2::GetPointAtUV( const Vec2& uvCoordsZeroToOne ) const
{
	Vec2 dimensions = GetDimensions();
	return min + ( uvCoordsZeroToOne * dimensions );
}

const Vec2 AABB2::GetUVForPoint( const Vec2& point ) const
{
	Vec2 uvPoint;
	uvPoint.x = RangeMapFloat( min.x, max.x, 0.f, 1.f, point.x );
	uvPoint.y = RangeMapFloat( min.y, max.y, 0.f, 1.f, point.y );
	return uvPoint;
}

AABB2 AABB2::GetBoxWithin( const Vec2& dimensions, const Vec2& alignment ) const
{
	Vec2 boxMaxPos = max - dimensions;
	Vec2 
}

void AABB2::operator=( const AABB2& assignFrom )
{
	min = assignFrom.min;
	max = assignFrom.max;
}

bool AABB2::operator==( const AABB2& compareWith ) const
{
	return ( min == compareWith.min ) && ( max == compareWith.max ) ;
}
