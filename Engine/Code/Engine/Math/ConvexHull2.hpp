#pragma once
#include "Engine/Math/Plane2.hpp"
#include <vector>

struct Vec2;
struct ConvexPoly2;

struct ConvexHull2 {
	std::vector<Plane2> m_planes;

public:
	ConvexHull2(){}
	~ConvexHull2(){}
	ConvexHull2( std::vector<Plane2> planes );

	static ConvexHull2 MakeConvexHullFromConvexPoly( ConvexPoly2 poly );
	std::pair<Vec2, Plane2> GetMostPossibleIntersectPointAndPlaneWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist = LONGEST_RAYCAST_DETECT_DIST ) const;
	// Accessor
	bool IsPointInside( Vec2 point ) const; 
	bool IsPointInsideWithoutPlane( Vec2 point, Plane2 plane ) const; 
};