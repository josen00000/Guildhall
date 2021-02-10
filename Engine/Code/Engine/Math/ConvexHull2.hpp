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
	explicit ConvexHull2( AABB2 box );
	explicit ConvexHull2( std::vector<Plane2> planes );

	static ConvexHull2 MakeConvexHullFromConvexPoly( ConvexPoly2 poly );
	static ConvexHull2 MakeConvexHullFromAABB( AABB2 box );

	// Accessor
	bool IsPointInside( Vec2 point ) const; 
	bool IsPointInsideWithoutPlane( Vec2 point, Plane2 plane ) const; 
	bool IsPointInsideWithoutPlanes( Vec2 point, std::vector<Plane2> planes ) const; 
	std::pair<Vec2, Plane2> GetMostPossibleIntersectPointAndPlaneWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist = LONGEST_RAYCAST_DETECT_DIST ) const;
	std::vector<Vec2> GetAllIntersectPoints() const;
	std::vector<Vec2> GetConvexPolyPoints() const;

	void AddPlane( Plane2 plane );
};