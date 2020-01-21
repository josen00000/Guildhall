#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Texture;

enum turretBehavior {
	SEARCH_PLAYER,
	ATTACT_PLAYER,
	NUM_BEHAVIOR
};

class NpcTurret :public Entity
{
	
public:
	NpcTurret(){}
	~NpcTurret();
	explicit NpcTurret(Map* map, Vec2 iniPos, EntityType entityType, EntityFaction entityFaction);

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;
	void ShootPlayer();

private:
	void CreateNpcTurret();
	void PatrolForDirection(Vec2 direction, float angularVelocityDegrees, float apertureDegrees);
	void RenderTurret() const;
	void RenderBarrel() const;
public:
	AABB2 m_turretShape;
	AABB2 m_barrelShape;
	std::vector <Vertex_PCU> m_turretVertices;
	std::vector <Vertex_PCU> m_barrelVertices;
	Texture* m_turretTexture=nullptr;
	Texture* m_barrelTexture=nullptr;
	float m_barrelAngularVelocity = TURRET_BARREL_ANGULAR_VELOCITY;
	float m_barrelOrientationDegrees = 0;
	SoundID npcTurretShoot = (size_t)-1;
	SoundID npcTurretHit = (size_t)-1;
	SoundID npcTurretDie = (size_t)-1;
	bool m_isPlayerVisiable = false;
	Vec2 m_targetPlayerLastDirection = Vec2::ZERO;
	Vec2 m_targetPlayerPos = Vec2(0,0);
	Vec2 m_targetDirection = Vec2(0,0);
	turretBehavior m_behavior = SEARCH_PLAYER;
	float m_shootCoolDown = 0.f;
	float m_patrolCoefficient = 1;
};

