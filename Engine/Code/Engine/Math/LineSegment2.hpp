#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/OBB2.hpp"

struct LineSegment2
{
	Vec2 m_start;
	Vec2 m_end;

public:
	LineSegment2()=default;
	~LineSegment2()=default;
	explicit LineSegment2( const Vec2& start, const Vec2& end );
	explicit LineSegment2( Vec2 const& start, Vec2 const& fwdDirt, float dist );

	static LineSegment2 ClipSegmentToSegment( LineSegment2 toClip, LineSegment2 refSeg );
	static LineSegment2 ClipSegmentToSegmentAlongDirection( LineSegment2 toClip, Vec2 start, Vec2 direction );

	//Accessor
	bool	IsPointMostlyInEdge( Vec2 point ) const;
	bool	IsPointMostlyInStraightLine( Vec2 point ) const;
	bool	IsLineOverlapWith( LineSegment2 line ) const;
	bool	IsLineParallelWith( LineSegment2 line ) const;
	bool	IsLineIntersectWithDisc( Vec2 center, float radius );
	float	GetLengthOfPointToLineSegment( const Vec2& refPos ) const; 
	float	GetLength() const;
	float	GetSlope() const;
	Vec2	GetStartPos() const;
	Vec2	GetEndPos() const;
	Vec2	GetNormalizedDirection() const;
	Vec2	GetDisplacement() const;
	Vec2	GetNearestPoint( const Vec2& refPos ) const;
	OBB2	GetLineOBB2WithThickness( float thickness ) const;

	//Mutator
	void	SetStartPos( const Vec2& start );
	void	SetEndPos( const Vec2& end );
};

