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

	// Accessor
	bool IsPointInside( Vec2 point ) const; 
};