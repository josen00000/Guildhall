#include "Cone3.hpp"

Cone3::Cone3( const Vec3& start, const Vec3& end, float radius )
	:m_start(start)
	,m_end(end)
	,m_radius(radius)
{
}

Cone3::Cone3( const Vec3& start, const Vec3& direction, float length, float radius )
	:m_start(start)
	,m_radius(radius)
{
	m_end = m_start + ( direction * length );
}

float Cone3::GetLength() const
{
	Vec3 disp = m_end - m_start;
	return disp.GetLength();
}

void Cone3::SetStartPos( const Vec3& start )
{
	m_start = start;
}

void Cone3::SetEndPos( const Vec3& end )
{
	m_end = end;
}

void Cone3::SetRadius( float radius )
{
	m_radius = radius;
}
