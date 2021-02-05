#include "ShapeUtils.hpp"
#include "Engine/Math/ConvexHull2.hpp"

RaycastResult2 Raycast2DWithConvexPolygons( std::vector<ConvexPoly2> const& polygons, Vec2 startPos, Vec2 forwardDirt )
{
	std::vector<RaycastResult2> results;
	for( int i = 0; i < polygons.size(); i++ ) {
		ConvexPoly2 tempPoly = polygons[i];
		RaycastResult2 tempResult = Raycast2DWithConvexPolygon( tempPoly, startPos, forwardDirt  );
		if( tempResult.m_didImpact ) {
			results.push_back( tempResult );
		}
	}

	RaycastResult2 shortestRaycastResult = results[0];
	for( RaycastResult2 tempResult : results ) {
		if( tempResult.m_impactDist < shortestRaycastResult.m_impactDist && tempResult.m_didImpact ) {
			shortestRaycastResult = tempResult;
		}
	}
	return shortestRaycastResult;
}

RaycastResult2 Raycast2DWithConvexPolygon( ConvexPoly2 const& polygon, Vec2 startPos, Vec2 forwardDirt )
{
	Vec2 polygonCenter = polygon.GetCenter();
	if( polygon.EarlyCheckDoesNeedRaycast( LineSegment2( startPos, forwardDirt ))) {
		ConvexHull2 tempHull = ConvexHull2::MakeConvexHullFromConvexPoly( polygon );
		//return Raycast2DWithConvexHull( tempHull, startPos, forwardDirt, LongestDist );
	
	}
	return RaycastResult2();
}

RaycastResult2 Raycast2DWithConvexHull( ConvexHull2 const& hull, Vec2 startPos, Vec2 forwardDirt )
{
	RaycastResult2 result;
// 	std::pair<Vec2, Plane2> possibleIntersectPointAndPlane = hull.GetMostPossibleIntersectPointAndPlaneWithRaycast( startPos, forwardDirt, LongestDist );
// 	if( hull.IsPointInsideWithoutPlane( possibleIntersectPointAndPlane.first, possibleIntersectPointAndPlane.second ) ) {
// 		result.m_didImpact = true;
// 		result.m_impactDist = 
// 	}
	return result;
}

RaycastResult2 Raycast2DWithConvexPolygonsWithDist( std::vector<ConvexPoly2> const& polygons, Vec2 startPos, Vec2 forwardDirt )
{
	return RaycastResult2();
}

RaycastResult2 Raycast2DWithConvexPolygonWithDist( ConvexPoly2 const& polygon, Vec2 startPos, Vec2 forwardDirt )
{
	return RaycastResult2();
}

RaycastResult2 Raycast2DWithConvexHullWithDist( ConvexHull2 const& hull, Vec2 startPos, Vec2 forwardDirt )
{
	return RaycastResult2();
}
