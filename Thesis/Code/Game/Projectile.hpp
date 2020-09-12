#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;

class Projectile {
public:
	Projectile( ActorType type, Vec2 startPos, Vec2 movingDirt );
	~Projectile();
	static Projectile* SpawnProjectileWithDirtAndType( Vec2 movingDirt, Vec2 startPos, ActorType type );
public:
	void UpdateProjectile( float deltaSeconds );
	void RenderProjectile();

	// Accessor

	// Mutator
private:

public:
	Rgba8 m_color;
	Vec2 m_pos = Vec2::ZERO;
	Vec2 m_movingDirection = Vec2::ZERO;
	float m_physicsRadiu	= 0.3f;
	float m_speed			= 5.f;
	ActorType	m_type		= ACTOR_NONE;
};

