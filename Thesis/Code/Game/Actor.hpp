#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"

class Game;
class Texture;
class Map;
class Texture;

enum ActorType {
	ACTOR_NONE,
	ACTOR_PLAYER,
	ACTOR_ENEMY
};

class Actor {
public:
	Actor();
	~Actor(){}
	explicit Actor( Vec2 pos );
	static Actor* SpawnActorWithPos( Vec2 pos );

public:
	virtual void UpdateActor( float deltaSeconds );
	virtual void RenderActor();
	virtual void Shoot( float deltaSeconds );
	void TakeDamage( float damage );
	void Die();

public:
	// Accessor
	bool GetIsPushedByActor() const { return m_isPushedByActor; }
	bool GetDoesPushActor() const { return m_doesPushActor; }
	bool GetIsDead() const { return m_isDead; }

	float GetOrientationDegrees() const { return m_orientationDegrees; } 
	float GetSpeed() const { return m_speed; }
	float GetPhysicsRadius() const { return m_physicsRadius; }
	float GetHealth() const { return m_hp; }
	float GetAttackStrength() const { return m_attackStrength; }

	Vec2 GetHeadDir() const; 
	Vec2 GetVelocity() const { return ( m_movingDir * m_speed ); }
	Vec2 GetPosition() const { return m_position; }

	// Mutator
	void SetOrientationDegrees( float orientationDegrees);
	void SetSpeed( float speed );
	void SetPhysicsRadius( float physicsRadius );
	void SetHealth( float hp );
	void SetAttackStrength( float attackStrength ) ;
	void SetMoveDirt( Vec2 moveDirt );								;
	void SetPosition( Vec2 pos );
	void SetBaseOBBSize( Vec2 size );
	void SetBarrelOBBSize( Vec2 size );

protected:
	bool m_isPushedByActor		= false;
	bool m_doesPushActor		= false;
	bool m_isDead				= false;

	ActorType m_type	= ActorType::ACTOR_NONE;

	float m_orientationDegrees	= 0.f;
	float m_speed				= 3.f;
	float m_physicsRadius		= 1.f;
	float m_hp					= 100.f;
	float m_attackStrength		= 10.f;
	float m_shootCoolDown		= 1.f;

	Vec2 m_movingDir		= Vec2::ZERO;
	Vec2 m_position			= Vec2::ZERO;
	Vec2 m_baseOBBSize		= Vec2::ZERO;
	Vec2 m_barrelOBBSize	= Vec2::ZERO;

	std::vector<Vertex_PCU> m_baseVertices;
	std::vector<Vertex_PCU> m_barrelVertices;

	Map* m_map = nullptr;
	Texture* m_texture = nullptr;

public:

	static Rgba8 s_enemyColor;
	static Rgba8 s_playerColor;
};

