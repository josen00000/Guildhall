#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Physics/RaycastResult.hpp"

class Texture;

enum TankBehavior {
	TANK_SEARCH_PLAYER,
	TAHK_ATTACT_PLAYER,
	TANK_NUM_BEHAVIOR
};

class NpcTank : public Entity
{
public:
	NpcTank() {}
	~NpcTank();
	explicit NpcTank( Map* map ,Vec2 spawnPos );
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;

private:
	void RenderTank() const;
	void CreateTank();
	bool IsTowardsToTargetDegrees(float targetDegrees, float apertureDegrees);
	void RotateToTargetDegrees(float targetDegrees, float angularSpeedsDegrees, bool ableMove);
	void UpdateRaycastResult();
	bool CheckCollisionWithWall();
	void handleCollisionWithWall();
	void Fire();

public:
	bool m_isPlayerVisiable = false;
	bool m_isEscapeFromWall = false;
	float m_escapeDist = 0 ;
	float m_escapeDegree = 0;
	float m_orientationDegrees = 0.f;
	float m_shootCoolDown = 0.f;
	float m_moveCoolDown = ENEMY_TANK_MOVE_COOL_DOWN;
	float m_targetDegrees = 0.f;
	float m_fireRange = 5.f;
	float m_escapeTime = 0.f;
	
	AABB2 m_tankShape;
	std::vector <Vertex_PCU> m_tankVertices;
	Texture* m_tankTexture=nullptr;
	TankBehavior m_behavior = TANK_SEARCH_PLAYER;
	Vec2 m_targetPlayerPos = Vec2( 0, 0 );
	Vec2 m_targetDirection = Vec2( 0, 0 );
	Vec2 m_targetLastPos = Vec2(0,0);
	SoundID npcTankShoot = (size_t)-1;
	SoundID npcTankHit = (size_t)-1;
	SoundID npcTankDie = (size_t)-1;
	RandomNumberGenerator m_rmg = RandomNumberGenerator();
	RaycastResult leftRaycastResult ;
	RaycastResult midRaycastResult ;
	RaycastResult rightRaycastResult;

};


