#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp" 
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"

struct LineSegment2;

struct ConvexPoly2{
public:
	std::vector<Vec2> m_points;

public:
	ConvexPoly2(){}
	~ConvexPoly2(){}
	ConvexPoly2( std::vector<Vec2> points );
	static ConvexPoly2 MakeConvexPolyFromPointCloud( std::vector<Vec2> points );


	// Accessor
	bool IsPointInside( Vec2 point ) const;
	bool EarlyCheckDoesNeedRaycast( LineSegment2 line ) const;

	Vec2 GetCenter() const;
	float GetBounderDiscRadius() const;
	std::vector<Vec2>& GetPoints(){ return m_points; }
};
