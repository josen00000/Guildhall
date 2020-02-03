#include "Disc2.hpp"

Disc2::Disc2( const Vec2& center, float radius )
	:m_center(center)
	,m_radius(radius)
{
}

void Disc2::SetCenter( const Vec2& center )
{
	m_center = center;
}

void Disc2::SetRadius( const float radius )
{
	m_radius = radius;
}
