#include "Disc2.hpp"
#include "Engine/Math/MathUtils.hpp"

Disc2::Disc2( const Vec2& center, float radius )
	:m_center(center)
	,m_radius(radius)
{
}

bool Disc2::IsIntersectWith( Vec2 pos, float radius )
{
	float distSqr = GetDistanceSquared2D( m_center, pos );
	if( distSqr <= ( m_radius + radius ) * ( m_radius + radius ) ){
		return true;
	}
	else{
		return false;
	}
}

bool Disc2::IsIntersectWith( Disc2 other )
{
	return  IsIntersectWith( other.GetCenter(), other.GetRadius() );
}

void Disc2::SetCenter( const Vec2& center )
{
	m_center = center;
}

void Disc2::SetRadius( const float radius )
{
	m_radius = radius;
}
