#pragma once
#include "Engine/Math/vec2.hpp"

struct RaycastResult
{
public:
	RaycastResult(){}
	~RaycastResult(){}
	explicit RaycastResult(bool DidImpact, float ImpactDist, int ImpactTileType, Vec2 ImpactPos , Vec2 start);/*, Vec2 ImpactNormal*/
public:
	bool m_didImpact = false;
	float m_impactDist = 0.f;
	int m_impactTileType = -1;
	Vec2 m_impactPos;
	Vec2 m_start;
	//Vec2 m_impactNormal;

};

