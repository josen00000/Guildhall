#include "Cylinder3.hpp"

Cylinder3::Cylinder3( const Vec3& start, const Vec3& end, float radius )
	:m_start(start)
	,m_end(end)
	,m_radius(radius)
{
}

Cylinder3::Cylinder3( const Vec3& start, const Vec3& direction, float length, float radius )
	:m_start(start)
	,m_radius(radius)
{
	m_end = m_start + direction * length;
}

float Cylinder3::GetLength() const
{
	Vec3 disp = m_end - m_start;
	return disp.GetLength();
}

float Cylinder3::GetlengthSquared() const
{
	Vec3 disp = m_end - m_start;
	return disp.GetLengthSquared();
}

void Cylinder3::SetStartPos( const Vec3& start )
{
	m_start = start;
}

void Cylinder3::SetEndPos( const Vec3& end )
{
	m_end = end;
}

void Cylinder3::SetRadius( float radius )
{
	m_radius = radius;
}
