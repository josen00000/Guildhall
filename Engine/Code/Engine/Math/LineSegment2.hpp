#pragma once
#include "Engine/Math/vec2.hpp"

struct LineSegment2
{
	Vec2 m_start;
	Vec2 m_end;

public:
	LineSegment2()=default;
	~LineSegment2()=default;
	explicit LineSegment2( const Vec2& start, const Vec2& end );

	static LineSegment2 ClipSegmentToSegment( LineSegment2 toClip, LineSegment2 refSeg );
	static LineSegment2 ClipSegmentToSegmentAlongDirection( LineSegment2 toClip, Vec2 start, Vec2 direction );

	//Accessor
	bool	IsPointMostlyInEdge( Vec2 point ) const;
	bool	IsPointMostlyInStraightLine( Vec2 point ) const;
	float	GetLengthOfPointToLineSegment( const Vec2& refPos ) const; 
	float	GetLength() const;
	float	GetSlope() const;
	Vec2	GetStartPos() const;
	Vec2	GetEndPos() const;
	Vec2	GetNormalizedDirection() const;
	Vec2	GetDirection() const;
	Vec2	GetNearestPoint( const Vec2& refPos ) const;

	//Mutator
	void	SetStartPos( const Vec2& start );
	void	SetEndPos( const Vec2& end );
};

