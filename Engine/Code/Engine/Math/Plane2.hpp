#pragma once
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"


struct Plane2 {
public:
	Vec2 m_normal;
	float m_dist;

public:
	Plane2();
	~Plane2(){}
	explicit Plane2( Vec2 normal, float dist );
	explicit Plane2( Vec2 normal, Vec2 pointOnPlane );
public:
	// Accessor
	bool	IsPointInFront( Vec2 point ) const;
	bool	IsPointInBack( Vec2 point ) const;
	bool	IsPossibleIntersectWith( LineSegment2 line ) const;
	bool	IsParallelWithPlane( Plane2 plane ) const;
	float	GetDistance() const { return m_dist; }
	float	GetSignedDistanceFromPlane( Vec2 point );
	float	GetDistanceFromPlane( Vec2 point );
	Vec2	GetNormal() const;
	Vec2	GetplaneOriginalPoint();
	Vec2	GetIntersectPointWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist = LONGEST_RAYCAST_DETECT_DIST );
	Vec2	GetIntersectPointWithPlane( Plane2 overlapPlane ) const;
	Plane2  GetFlipped();

	// Mutator
	void SetNormal( Vec2 normal );
	void Flip();

	//operator
	bool operator==( const Plane2& plane ) const;
};