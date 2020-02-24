#pragma once
#include "Engine/Math/Vec2.hpp"

struct Disc2 {
	Vec2 m_center	= Vec2::ZERO;
	float m_radius	= 1.f;

public:
	Disc2(){}
	~Disc2(){}
	explicit Disc2( const Vec2& center, float radius );

public:
	// Accessor
	Vec2 GetCenter() const { return m_center; }
	float GetRadius() const { return m_radius; }
	bool IsIntersectWith( Vec2 pos, float radius );
	bool IsIntersectWith( Disc2 other );

	//Mutator
	void SetCenter( const Vec2& center );
	void SetRadius( const float radius );
};