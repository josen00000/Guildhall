#include "ShapeUtils.hpp"
#include "Engine/Math/ConvexHull2.hpp"


RaycastResult2 Raycast2DWithConvexPolygons( std::vector<ConvexPoly2> const& polygons, Vec2 startPos, Vec2 forwardDirt, float dist /*= LONGEST_RAYCAST_DETECT_DIST */ )
{
	RaycastResult2 result;
	float minDist = -1;
	for( int i = 0; i < polygons.size(); i++ ) {
		ConvexPoly2 tempPoly = polygons[i];
		RaycastResult2 tempResult = Raycast2DWithConvexPolygon( tempPoly, startPos, forwardDirt, dist );
		if( tempResult.m_didImpact ) {
			if( minDist < 0 ) {
				minDist = tempResult.m_impactDist;
			}
			if( tempResult.m_impactDist <= minDist ) {
				result = tempResult;
				minDist = tempResult.m_impactDist;
			}
		}
	}
	return result;
}

RaycastResult2 Raycast2DWithConvexPolygon( ConvexPoly2 const& polygon, Vec2 startPos, Vec2 forwardDirt, float dist /*= LONGEST_RAYCAST_DETECT_DIST */ )
{
	Vec2 polygonCenter = polygon.GetCenter();
	if( polygon.IsPossibleIntersectWithLine( LineSegment2( startPos, forwardDirt, dist ) ) ) {
		ConvexHull2 tempHull = ConvexHull2::MakeConvexHullFromConvexPoly( polygon );
		return Raycast2DWithConvexHull( tempHull, startPos, forwardDirt, dist );
	}
	return RaycastResult2();
}

RaycastResult2 Raycast2DWithConvexHull( ConvexHull2 const& hull, Vec2 startPos, Vec2 forwardDirt, float dist /*= LONGEST_RAYCAST_DETECT_DIST */ )
{
	RaycastResult2 result;
	std::pair<Vec2, Plane2> possibleIntersectPointAndPlane = hull.GetMostPossibleIntersectPointAndPlaneWithRaycast( startPos, forwardDirt );
	float possibleDist = GetDistance2D( possibleIntersectPointAndPlane.first, startPos );
	if( possibleDist < dist &&  hull.IsPointInsideWithoutPlane( possibleIntersectPointAndPlane.first, possibleIntersectPointAndPlane.second ) ) {
		result.m_didImpact = true;
		result.m_impactDist = possibleDist;
		result.m_impactPos = possibleIntersectPointAndPlane.first;
		result.m_start = startPos;
	}
	return result;
}

