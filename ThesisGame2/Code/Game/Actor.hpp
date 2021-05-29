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

enum AliveState :uint {
	ALIVE,
	WAIT_FOR_REBORN,
	WAIT_FOR_DELETE,
	READY_TO_DELETE_CONTROLLER,
	READY_TO_DELETE_PLAYER,
	NUM_OF_ALIVE_STATE
};

class Actor {
public:
	Actor();
	~Actor(){}
	explicit Actor( Vec2 pos );
	static Actor* SpawnActorWithPos( Vec2 pos );

public:
	virtual void UpdateActor( float deltaSeconds, Rgba8 color = Rgba8::WHITE );
	virtual void RenderActor();
	virtual void Shoot( float deltaSeconds );
	virtual void TakeDamage( float damage );
	virtual void Die();

public:
	// Accessor
	bool GetIsPushedByActor() const { return m_isPushedByActor; }
	bool GetDoesPushActor() const { return m_doesPushActor; }

	AliveState GetAliveState() const { return m_aliveState; }

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
	void SetAliveState( AliveState state );

protected:
	bool m_isPushedByActor		= false;
	bool m_doesPushActor		= false;
	Rgba8 m_color				= Rgba8::WHITE;

	ActorType m_type		= ActorType::ACTOR_NONE;
	AliveState m_aliveState = AliveState::ALIVE;

	float m_orientationDegrees		= 0.f;
	float m_speed					= 3.f;
	float m_physicsRadius			= 1.f;
	float m_hp						= 1000.f;
	float m_attackStrength			= 10.f;
	float m_shootCoolDown			= 0.3f;
	float m_shootTimer				= m_shootCoolDown;
	float m_lastFrameMovingDegrees	= 0.f;

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

