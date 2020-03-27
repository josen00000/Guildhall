#pragma once
#include "Game/GameObject.hpp"
#include "Engine/Math/AABB3.hpp"

class GPUMesh;


class CubeSphere : GameObject {
public:
	CubeSphere( RenderContext* ctx, int level, AABB3 cube );
	~CubeSphere();

	//virtual void Render();

public:
	int m_level;
	AABB3 m_cube;
};