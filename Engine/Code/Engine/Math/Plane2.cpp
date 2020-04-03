#include "Plane2.hpp"
#include "Engine/Math/MathUtils.hpp"


Plane2::Plane2()
	:m_normal(Vec2( 0, 1 ))
	,m_dist(0)
{
}

Plane2::Plane2( Vec2 normal, Vec2 pointOnPlane )
	:m_normal(normal)
	,m_dist(DotProduct2D( pointOnPlane, normal ))
{
	
}

bool Plane2::IsPointInFront( Vec2 point )
{
	float pointDistFromPlane = DotProduct2D( point, m_normal );
	float distFromPlane = pointDistFromPlane - m_dist;
	return distFromPlane > 0.0f;
}

bool Plane2::IsPointInBack( Vec2 point )
{
	return !IsPointInFront( point );
}

float Plane2::GetSignedDistanceFromPlane( Vec2 point )
{
	float pointDistFromPlane = DotProduct2D( point, m_normal );
	float distFromPlane = pointDistFromPlane - m_dist;
	return distFromPlane;
}

float Plane2::GetDistanceFromPlane( Vec2 point )
{
	float signedDist = GetSignedDistanceFromPlane( point );
	return abs( signedDist );
}

Vec2 Plane2::GetplaneOriginalPoint()
{
	return m_normal * m_dist;
}
