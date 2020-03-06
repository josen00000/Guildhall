#pragma once
#include "Engine/Math/vec2.hpp"

class Collider2D;

struct Manifold2D {
	float dist;
	Vec2 normal;
};

struct Collision2D {
public:
	Collider2D* me;
	Collider2D* other;
	Manifold2D	manifold;

public:
	Collision2D( Collider2D* colA, Collider2D* colB, Manifold2D manifold );
	// Accessor
	float GetDist() const;
	Vec2 GetNormal() const;
	Vec2 GetTangent() const;
	Manifold2D GetManifold() const;
};