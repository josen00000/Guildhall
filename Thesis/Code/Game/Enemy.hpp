#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;
class Player;

enum EnemyAIState {
	ENEMY_PATROL,
	ENEMY_ATTACK,
	NUM_OF_ENEMY_STATE
};

class Enemy : public Actor {
public:
	Enemy();
	~Enemy(){}
	
public:
	static Enemy* SpawnPlayerWithPos( Map* map,  Vec2 pos );

public:
	void UpdateEnemy( float deltaSeconds );
	void RenderEnemy();

	// Accessor
	// Mutator
	void SetMap( Map* map );

	void CheckState();
	void CheckIfReachGoal();

private:
	void FindGoalPatrolPoint( float deltaSeconds );
	virtual void Shoot( float deltaSeconds ) override;

public:
	float m_maxSpeed		= 3.f;
	Rgba8 m_color			= Rgba8::BLUE;

private:
	float m_activeDistThreshold = 10.f;
	EnemyAIState m_state	= ENEMY_PATROL;
	Vec2 m_patrolGoalPos	= Vec2::ZERO;
	Player*	m_target		= nullptr;
};

