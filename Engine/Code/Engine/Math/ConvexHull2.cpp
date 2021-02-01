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


bool ConvexHull2::IsPointInside( Vec2 point ) const
{
	for( Plane2 plane : m_planes ) {
		if( plane.IsPointInFront( point ) ) {
			return false;
		}
	}
	return true;
}
