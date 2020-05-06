#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/LineSegment2.hpp"

class Collider2D;

struct Manifold2D {
	float dist;
	Vec2 normal;
	LineSegment2 contact;
};

struct Collision2D {
public:
	Collider2D* me;
	Collider2D* other;
	Manifold2D	manifold;
	bool m_needResolve = true;

public:
	Collision2D( Collider2D* colA, Collider2D* colB, Manifold2D manifold );
	
	// Accessor
	float GetDist() const;
	Vec2 GetNormal() const;
	Vec2 GetTangent() const;
	Vec2 GetContactCenter() const;
	LineSegment2 GetContact() const { return manifold.contact; }
	Manifold2D GetManifold() const;

	// Mutator
	void SetContactAsVec2( Vec2 point );
	void SetContact( LineSegment2 line );
};