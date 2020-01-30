#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"


class DiscCollider2D : public Collider2D {
public:

private:
	Vec2 m_localPosition;
	Vec2 m_worldPosition;
	float m_radius;
};