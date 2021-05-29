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

Plane2::Plane2( Vec2 normal, float dist )
	:m_normal(normal)
	,m_dist(dist)
{
}

bool Plane2::IsPointInFront( Vec2 point ) const
{
	float pointDistFromPlane = DotProduct2D( point, m_normal );
	return pointDistFromPlane > m_dist;
}

bool Plane2::IsPointInBack( Vec2 point ) const
{
	return !IsPointInFront( point );
}

bool Plane2::IsPossibleIntersectWith( LineSegment2 line ) const
{
	Vec2 fwdDirt = line.GetNormalizedDirection();
	Vec2 centerPointInPlane = m_dist * m_normal;
	Vec2 lineStartInPlaneCoords = line.GetStartPos() - centerPointInPlane;
	Vec2 lineEndInPlaneCoords = line.GetEndPos() - centerPointInPlane;
	float lineStartInPlaneCoordsY = DotProduct2D( lineStartInPlaneCoords, m_normal );
	float lineEndInPlaneCoordsY = DotProduct2D( lineEndInPlaneCoords, m_normal );
	return lineStartInPlaneCoordsY * lineEndInPlaneCoordsY <= 0 ;
}

bool Plane2::IsParallelWithPlane( Plane2 plane ) const
{
	return ( IsVec2MostlyEqual( m_normal, plane.m_normal ) || IsVec2MostlyEqual( -m_normal, plane.m_normal ) );
}

bool Plane2::IsFlippedWith( Plane2 plane )
{
	return IsFloatMostlyEqual( m_dist, -plane.m_dist ) && IsVec2MostlyEqual( m_normal, -plane.m_normal );
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

Vec2 Plane2::GetNormal() const
{
	return m_normal;
}

Vec2 Plane2::GetplaneOriginalPoint()
{
	return m_normal * m_dist;
}

Vec2 Plane2::GetIntersectPointWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist /*= LONGEST_RAYCAST_DETECT_DIST */ )
{
	UNUSED(maxDist)
	// intersect point P
	// plane center point N.
	float DistOSInPlaneNormal = DotProduct2D( startPos, m_normal );
	float distSN = abs( DistOSInPlaneNormal - m_dist );
	Vec2 SNNormalized = m_normal;

	float distProjectSNNormalizedToSP = abs( DotProduct2D( SNNormalized, fwdDirt ) );

	// SP / distprojectsnnormalziedtosp = dist / 1
	float SPDist = distSN / distProjectSNNormalizedToSP  ;
	return startPos + fwdDirt * SPDist;
}

Vec2 Plane2::GetIntersectPointWithPlane( Plane2 overlapPlane ) const
{
	float a1 = m_normal.x;
	float b1 = m_normal.y;
	float c1 = m_dist;

	float a2 = overlapPlane.m_normal.x;
	float b2 = overlapPlane.m_normal.y;
	float c2 = overlapPlane.m_dist;

	float determinant = a1 * b2 - a2 * b1;

	if( determinant == 0 ) {
		return Vec2( NAN, NAN );
	}
	else {
		Vec2 result;
		result.x = ((b2 * c1) - (b1 * c2)) / determinant;
		result.y = ((a1 * c2) - (a2 * c1)) / determinant;
		return result;
	}
}

Plane2 Plane2::GetFlipped()
{
	return Plane2( -m_normal, -m_dist );
}

void Plane2::SetNormal( Vec2 normal )
{
	m_normal = normal;
}

void Plane2::Flip()
{
	m_normal = -m_normal;
	m_dist = -m_dist;
}

// Operator
bool Plane2::operator==( const Plane2& plane ) const
{
	return IsFloatMostlyEqual( m_dist, plane.GetDistance()) && IsVec2MostlyEqual( m_normal, plane.GetNormal() );
}
