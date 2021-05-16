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
	void Die();

	// Accessor
	bool GetIsDead() const { return m_isDead; }
	float GetDamage() const { return m_damage; }
	ActorType GetType() const { return m_type; }
	Vec2 GetPosition() const { return m_pos; }


	// Mutator
	void SetColor( Rgba8 color );
	void SetDamage( float damage );
private:
	bool m_isDead = false;

public: 
	Rgba8 m_color;
	float m_physicsRadiu	= 0.3f;
	float m_speed			= 8.f;
	float m_damage			= 10.f;
	ActorType	m_type		= ACTOR_NONE;

	Vec2 m_pos = Vec2::ZERO;
	Vec2 m_movingDirection = Vec2::ZERO;

	Texture* m_texture = nullptr;
	std::vector<Vertex_PCU> m_vertices;
};

