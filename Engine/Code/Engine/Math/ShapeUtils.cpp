#include "ShapeUtils.hpp"

RaycastResult2 Raycast2DWithPolygons( Vec2 startPos, Vec2 forwardDirt, std::vector<ConvexPoly2*> const& polygons )
{
	std::vector<RaycastResult2> results;
	for( int i = 0; i < polygons.size(); i++ ) {
		ConvexPoly2* tempPoly = polygons[i];
		RaycastResult2 tempResult = Raycast2DWithPolygon( startPos, forwardDirt, *tempPoly );
		results.push_back( tempResult );
	}

	RaycastResult2 shortestRaycastResult = results[0];
	for( RaycastResult2 tempResult : results ) {
		if( tempResult.m_impactDist < shortestRaycastResult.m_impactDist && tempResult.m_didImpact ) {
			shortestRaycastResult = tempResult;
		}
	}
	return shortestRaycastResult;
}

RaycastResult2 Raycast2DWithPolygon( Vec2 startPos, Vec2 forwardDirt, ConvexPoly2 const& polygons )
{
	return RaycastResult2();
}
