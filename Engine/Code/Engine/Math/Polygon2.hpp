#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp" 
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/ConvexHull2.hpp"

struct Polygon2
{
public:
	std::vector<LineSegment2> m_edges; // counter-clockwise
	Vec2 m_center;
public:
	Polygon2() = default;
	~Polygon2() = default;	
	Polygon2( std::vector<Vec2> points );
	static Polygon2 MakeConvexFromPointCloud( std::vector<Vec2> points );
	static Polygon2 MakeConvexFromAABB2( AABB2 box );
	static Polygon2 MakeConvexPolyFromConvexHull( ConvexHull2 hull );
public:
	// bool
	bool IsValid() const;
	bool IsConvex() const;

	bool IsPointInside( Vec2 point ) const;

	// Accessor
	float	GetDistanceToCenter( Vec2 point ) const;
	float	GetLongestDistance() const;
	float	GetShortestDistanceToEdge( Vec2 point ) const;
	float	GetArea() const;
	int		GetVertexCount() const;
	int		GetEdgeCount() const;

	Vec2	GetLowestPoint() const;
	Vec2	GetClosestPoint( Vec2 point ) const;
	Vec2	GetClosestPointOnEdges( Vec2 point ) const;
	Vec2	GetCenter( std::vector<Vec2> rawPoints ) const;
	Vec2	GetCenter() const { return m_center; }
	Vec2	GetEdgeNormal( int edgeIndex ) const;
	std::pair<Vec2, Vec2> GetIntersectPointWithStraightLine( LineSegment2 line );
	std::vector<Vec2> GetAllPoints();

	bool GetEdgeInWorldWithPoint( Vec2 point,  LineSegment2& seg ) const;

	LineSegment2 GetEdge( int index ) const;
	LineSegment2 GetEdgeInWorld( int index ) const;
	LineSegment2 GetEdgeInWorldWithPoint( Vec2 point ) const;

	AABB2 GetQuickOutBox();

	int		GetEdgeIndexWithPoint( Vec2 point ) const;
	void	GetAllVerticesInWorld( std::vector<Vec2>& vertices ) const;

	// Mutator
	void	SetEdgesFromPoints( std::vector<Vec2> points );
	void	SetCenter( Vec2 center );
	bool	ReplacePointWithPoint( Vec2 replacedPoint, Vec2 newPoint );

	void Normalize();


};
