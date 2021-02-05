#include "ConvexHull2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"

ConvexHull2::ConvexHull2( std::vector<Plane2> planes )
{
	m_planes = planes;
}

ConvexHull2 ConvexHull2::MakeConvexHullFromConvexPoly( ConvexPoly2 poly )
{
	std::vector<Plane2> planes;
	for( int i = 0; i < poly.m_points.size() - 1; i++ ) {
		Vec2 forward = poly.m_points[i+1] - poly.m_points[i];
		Vec2 normal = forward.GetRotatedMinus90Degrees();
		normal.Normalize();
		Plane2 tempPlane = Plane2( normal, poly.m_points[i] );
		planes.push_back( tempPlane );
	}
	Vec2 forward = poly.m_points.front() - poly.m_points.back();
	Vec2 normal = forward.GetRotatedMinus90Degrees();
	normal.Normalize();
	Plane2 tempPlane = Plane2( normal, poly.m_points.front() ); 
	planes.push_back( tempPlane );
	return ConvexHull2( planes );
}

std::pair<Vec2, Plane2> ConvexHull2::GetMostPossibleIntersectPointAndPlaneWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist /*= LONGEST_RAYCAST_DETECT_DIST */ ) const
{
	Vec2 resultIntersectPoint;
	float LargestDistSQ = 0;
	Plane2 candidatePlane;

	float longestDistSq = 0;
	for( Plane2 plane : m_planes ) {
		if( plane.IsPointInBack( startPos ) ) { continue; }
		
		Vec2 tempIntersectPoint = plane.GetIntersectPointWithRaycast( startPos, fwdDirt, maxDist );
		float distSQ = GetDistanceSquared2D( tempIntersectPoint, startPos );
		if( distSQ > LargestDistSQ ) {
			LargestDistSQ = distSQ;
			candidatePlane = plane;
			resultIntersectPoint = tempIntersectPoint;
		}
	}
	return std::pair<Vec2, Plane2>( resultIntersectPoint, candidatePlane );
}

bool ConvexHull2::IsPointInside( Vec2 point ) const
{
	for( Plane2 plane : m_planes ) {
		if( plane.IsPointInFront( point ) ) {
			return false;
		}
	}
	return true;
}


bool ConvexHull2::IsPointInsideWithoutPlane( Vec2 point, Plane2 plane ) const
{
	for( Plane2 tempPlane : m_planes ) {
		if( tempPlane == plane ) { continue; }
		if( plane.IsPointInFront( point ) ) { return false; }
	}
	return true;
}
