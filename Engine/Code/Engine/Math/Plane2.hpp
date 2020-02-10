#pragma once
#include "Engine/Math/Vec2.hpp"

struct Disc2 {
public:
	Vec2 normal;
	float distFromOriginAlongNormal;

public:
	float GetDistance() const { return distFromOriginAlongNormal; }
};