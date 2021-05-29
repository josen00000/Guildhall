#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;

class Projectile {
public:
	Projectile( int playerIndex, Vec2 startPos, Vec2 movingDirt );
	~Projectile();
	static Projectile* SpawnProjectileWithDirtAndType( Vec2 movingDirt, Vec2 startPos, int playerIndex );
public:
	void UpdateProjectile( float deltaSeconds );
	void RenderProjectile();
	void Die();

	// Accessor
	int GetPlayerIndex() const { return m_playerIndex; }
	bool GetIsDead() const { return m_isDead; }
	bool IsMovingTorwards( Vec2 point ) const;
	float GetDamage() const { return m_damage; }
	Vec2 GetPosition() const { return m_pos; }


	// Mutator
	void SetColor( Rgba8 color );
private:
	bool m_isDead = false;

public: 
	Rgba8 m_color;
	int m_playerIndex		= 0;
	float m_physicsRadiu	= 0.3f;
	float m_speed			= 10.f;
	float m_damage			= 10.f;

	Vec2 m_pos = Vec2::ZERO;
	Vec2 m_movingDirection = Vec2::ZERO;

	Texture* m_texture = nullptr;
	std::vector<Vertex_PCU> m_vertices;
};

