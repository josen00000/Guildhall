#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp" 
#include "Engine/Math/LineSegment2.hpp"


struct Polygon2
{
private:
	std::vector<LineSegment2> m_edges;
	std::vector<Vec2> m_disps;
	Vec2 m_center;

public:
	Polygon2(){}
	~Polygon2(){}
	Polygon2( std::vector<Vec2> points );
	static Polygon2 MakeConvexFromPointCloud( std::vector<Vec2> points );
public:
	// bool
	bool IsValid() const;
	bool IsConvex() const;

	bool Contains( Vec2 point ) const;

	// Accessor
	float	GetDistance( Vec2 point ) const;
	int		GetVertexCount() const;
	int		GetEdgeCount() const;
	Vec2	GetClosestPoint( Vec2 point ) const;
	Vec2	GetBadCenter() const { return m_center; }
	LineSegment2 GetEdge( int index );

	// Mutator
	void	SetEdgesFromPoints( const Vec2* point, int pointCount );
	void	SetCenter( Vec2 center );

private:
	void	InitialBadCenter();
	void	RecordDispOfCenterAndPoints();
	void	UpdatePoints();
};
