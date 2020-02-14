#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp" 
#include "Engine/Math/LineSegment2.hpp"


struct Polygon2
{
public:
	std::vector<LineSegment2> m_edges;
	Vec2 m_center;
public:
	Polygon2() = default;
	~Polygon2() = default;	
	Polygon2( std::vector<Vec2> points );
	static Polygon2 MakeConvexFromPointCloud( std::vector<Vec2> points );
public:
	// bool
	bool IsValid() const;
	bool IsConvex() const;

	bool Contains( Vec2 point ) const;

	// Accessor
	float	GetDistance( Vec2 point ) const;
	float	GetLongestDistance() const;
	Vec2	GetLowestPoint() const;
	int		GetVertexCount() const;
	int		GetEdgeCount() const;
	Vec2	GetClosestPoint( Vec2 point ) const;
	Vec2	GetBadCenter( std::vector<Vec2> rawPoints ) const;
	Vec2	GetMassCenter() const { return m_center; }
	LineSegment2 GetEdge( int index );

	// Mutator
	void	SetEdgesFromPoints( std::vector<Vec2> points );
	void	SetCenter( Vec2 center );

	void Normalize();


};
