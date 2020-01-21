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

	//Accessor
	Vec2	GetStartPos() const;
	Vec2	GetEndPos() const;
	Vec2	GetNormalizedDirection() const;
	float	GetLengthOfPointToLineSegment( const Vec2& refPos ) const; 
	float	GetLength() const;
	Vec2	GetNearestPoint( const Vec2& refPos ) const;

	//Mutator
	void	SetStartPos( const Vec2& start );
	void	SetEndPos( const Vec2& end );
};

