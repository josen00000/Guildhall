#pragma once
#include <vector>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

class Game;
class Texture;
class Map;

enum ActorType {
	ACTOR_NONE,
	ACTOR_PLAYER,
	ACTOR_ENEMY
};

class Actor {
public:
	Actor(){}
	~Actor(){}
	explicit Actor( Vec2 pos );
	static Actor* SpawnActorWithPos( Vec2 pos );

public:
	virtual void UpdateActor( float deltaSeconds );
	virtual void RenderActor();
	virtual void Shoot();
	void TakeDamage( float damage );

public:
	// Accessor
	float GetOrientationDegrees() const { return m_orientationDegrees; } 
	float GetSpeed() const { return m_speed; }
	float GetPhysicRadius() const { return m_physicsRadius; }
	float GetHealth() const { return m_hp; }
	float GetAttackStrength() const { return m_attackStrength; }

	Vec2 GetPosition() const { return m_position; }

	// Mutator
	void SetOrientationDegrees( float orientationDegrees);
	void SetSpeed( float speed );
	void SetPhysicsRadius( float physicsRadius );
	void SetHealth( float hp );
	void SetAttackStrength( float attackStrength ) ;
	void SetMoveDirt( Vec2 moveDirt );								;
	void SetPosition( Vec2 pos );


protected:
	float m_orientationDegrees	= 0.f;
	float m_speed				= 2.f;
	float m_physicsRadius		= 1.f;
	float m_hp					= 100.f;
	float m_attackStrength		= 10.f;


	Vec2 m_movingDirt	= Vec2::ZERO;
	Vec2 m_position		= Vec2::ZERO;

	ActorType m_type	= ActorType::ACTOR_NONE;

	Map* m_map = nullptr;
public:

	static Rgba8 s_enemyColor;
	static Rgba8 s_playerColor;
};

