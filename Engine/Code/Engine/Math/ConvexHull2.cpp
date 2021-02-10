#include "ConvexHull2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/ConvexPoly2.hpp"

ConvexHull2::ConvexHull2( std::vector<Plane2> planes )
{
	m_planes = planes;
}

ConvexHull2::ConvexHull2( AABB2 box )
{
	m_planes.push_back( Plane2( Vec2::DOWN_NORMAL_DIRECTION, box.mins ));
	m_planes.push_back( Plane2( Vec2::UP_NORMAL_DIRECTION, box.maxs ));
	m_planes.push_back( Plane2( Vec2::LEFT_NORMAL_DIRECTION, box.mins ));
	m_planes.push_back( Plane2( Vec2::RIGHT_NORMAL_DIRECTION, box.maxs ));
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

ConvexHull2 ConvexHull2::MakeConvexHullFromAABB( AABB2 box )
{
	// abcd. a left down cornor, b left right cornor, c right up cornor, d leftup cornor
	Vec2 cornors[4];
	box.GetCornerPositions( cornors );
	std::vector<Vec2> tangents;
	tangents.push_back( (Vec2( box.maxs.x, box.mins.y ) - box.mins).GetNormalized());
	tangents.push_back( (box.maxs - Vec2( box.maxs.x, box.mins.y )).GetNormalized());
	tangents.push_back( (Vec2( box.mins.x, box.maxs.y ) - box.maxs).GetNormalized());
	tangents.push_back( (box.mins - Vec2( box.mins.x, box.maxs.y )).GetNormalized());

	std::vector<Plane2> convexHullPlanes;
	
	for( int i = 0; i < tangents.size(); i++ ) {
		Vec2 normal = tangents[i];
		convexHullPlanes.push_back( Plane2( normal, cornors[i] ));
	}
	return ConvexHull2( convexHullPlanes );
}

std::pair<Vec2, Plane2> ConvexHull2::GetMostPossibleIntersectPointAndPlaneWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist /*= LONGEST_RAYCAST_DETECT_DIST */ ) const
{
	Vec2 resultIntersectPoint;
	float LargestDistSQ = 0;
	Plane2 candidatePlane;

	for( Plane2 plane : m_planes ) {
		if( plane.IsPointInBack( startPos ) ) { continue; }
		if( !plane.IsPossibleIntersectWith( LineSegment2( startPos, fwdDirt, maxDist ))){ continue; }
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

std::vector<Vec2> ConvexHull2::GetAllIntersectPoints() const
{
	std::vector<Vec2> result;
	for( int i = 0; i < m_planes.size() - 1; i++ ) {
		for( int j = i + 1; j < m_planes.size(); j++ ) {
			Vec2 intersectPoint = m_planes[i].GetIntersectPointWithPlane( m_planes[j] );
			result.push_back( intersectPoint );
		}
	}
	return result;
}

std::vector<Vec2> ConvexHull2::GetConvexPolyPoints() const
{
	std::vector<Vec2> result;
	for( int i = 0; i < m_planes.size() - 1; i++ ) {
		for( int j = i + 1; j < m_planes.size(); j++ ){
			if( m_planes[i].IsParallelWithPlane( m_planes[j] ) ){ continue; }
			Vec2 intersectPoint = m_planes[i].GetIntersectPointWithPlane( m_planes[j] );
			std::vector<Plane2> skippedPlanes { m_planes[i], m_planes[j] };
			if( IsPointInsideWithoutPlanes( intersectPoint, skippedPlanes ) ) {
				result.push_back( intersectPoint );
			}
		}
	}
	return result;
}

void ConvexHull2::AddPlane( Plane2 plane )
{
	m_planes.push_back( plane );
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
		if( tempPlane.IsPointInFront( point ) ) { return false; }
	}
	return true;
}

bool ConvexHull2::IsPointInsideWithoutPlanes( Vec2 point, std::vector<Plane2> planes ) const
{
	for( Plane2 tempPlane : m_planes ) {
		bool isPlaneSkipped = false;
		for( Plane2 tempCheckPlane : planes ) {
			if( tempCheckPlane == tempPlane ){ 
				isPlaneSkipped = true; 
				break;
			}
		}
		if( isPlaneSkipped ){ continue; }
		if( tempPlane.IsPointInFront( point ) ){ return false; }
	}
	return true;
}
