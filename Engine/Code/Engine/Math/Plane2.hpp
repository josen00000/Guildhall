#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/EngineCommon.hpp"


struct Plane2 {
public:
	Vec2 m_normal;
	float m_dist;

public:
	Plane2();
	~Plane2(){}
	Plane2( Vec2 normal, Vec2 pointOnPlane );
public:
	// Accessor
	bool	IsPointInFront( Vec2 point );
	bool	IsPointInBack( Vec2 point );
	float	GetDistance() const { return m_dist; }
	float	GetSignedDistanceFromPlane( Vec2 point );
	float	GetDistanceFromPlane( Vec2 point );
	Vec2	GetNromal() const;
	Vec2	GetplaneOriginalPoint();
	Vec2	GetIntersectPointWithRaycast( Vec2 startPos, Vec2 fwdDirt, float maxDist = LONGEST_RAYCAST_DETECT_DIST );

	//operator
	bool operator==( const Plane2& plane ) const;
};