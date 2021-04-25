#pragma once
#include "Game/Map/Map.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"

class Game;

class Item {
public:
	Item(){}
	~Item(){}
	Item( Vec2 spawnPos );

	Vec2 GetPosition() const { return m_pos; }

	void SetPosition( Vec2 pos );
	void RenderItem();

public:
	Vec2 m_pos;
	AABB2 m_shape;
};