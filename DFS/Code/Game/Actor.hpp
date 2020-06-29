#pragma once
#include <vector>
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

class Game;
class Texture;

class Actor {
public:
	Actor(){}
	~Actor(){}
	explicit Actor( ActorDefinition const& definition );
	static Actor* SpawnActorWithPos( ActorDefinition const& definition, Vec2 pos );
public:
	virtual void UpdateActor( float deltaSeconds );
	virtual void UpdateActorVerts( Vec2 uvAtMin, Vec2 uvAtMax );
	virtual void RenderActor();
	void UpdateActorAnimation( float deltaSeconds );

public:
	// Accessor
	float GetOrientationDegrees() const { return m_orientationDegrees; } 
	float GetSpeed() const { return m_speed; }
	float GetPhysicRadius() const { return m_physicsRadius; }
	float GetHealth() const { return m_hp; }
	float GetAttackStrength() const { return m_attackStrength; }

	Vec2 GetPosition() const { return m_position; }
	AABB2 GetShape() const { return m_shape; }

	// Mutator
	void SetOrientationDegrees( float orientationDegrees);
	void SetSpeed( float speed );
	void SetPhysicsRadius( float physicsRadius );
	void SetHealth( float hp );
	void SetAttackStrength( float attackStrength ) ;
								;
	void SetPosition( Vec2 pos );
	void SetShape( AABB2 const& shape );


protected:
	bool m_isMoving = false;

	float m_orientationDegrees	= 0.f;
	float m_speed				= 0.f;
	float m_physicsRadius		= 0.f;
	float m_hp					= 0.f;
	float m_attackStrength		= 0.f;


	Vec2 m_position		= Vec2::ZERO;
	AABB2 m_shape		= AABB2();
	Texture* m_texture	= nullptr;
	ActorDefinition	m_definition;


	std::vector<Vertex_PCU> m_verts;
};

