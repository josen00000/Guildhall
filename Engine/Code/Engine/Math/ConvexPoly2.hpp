#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp" 
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"

struct LineSegment2;
struct ConvexHull2;

struct ConvexPoly2{
public:
	std::vector<Vec2> m_points;

public:
	ConvexPoly2(){}
	~ConvexPoly2(){}
	ConvexPoly2( std::vector<Vec2> points );
	static ConvexPoly2 MakeConvexPolyFromPointCloud( std::vector<Vec2> points );
	static ConvexPoly2 MakeConvexPolyFromConvexHull( ConvexHull2 hull );
	static ConvexPoly2 MakeConvexPolyFromAABB2( AABB2 const& box );

	// Accessor
	bool IsPointInside( Vec2 point ) const;
	bool IsPossibleIntersectWithLine( LineSegment2 line ) const;

	void GetPoints( std::vector<Vec2>& points ) const;
	void GetEdges( std::vector<LineSegment2>& edges ) const;
	int GetPointsCount() const {  return (int)m_points.size(); }
	float GetLongestDistance() const;
	float GetShortestDistanceToEdge( Vec2 const& point ) const;
	float GetBounderDiscRadius() const;
	float GetArea() const;
	Vec2 GetCenter() const;
 	Vec2 GetClosestPointOnEdges( Vec2 const& point ) const;

	// Mutator
	void SetPoints( std::vector<Vec2>& points);
	void SetCenterPos( Vec2 const& center );
};
