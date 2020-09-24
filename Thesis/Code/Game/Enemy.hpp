#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;

enum EnemyAIState {
	ENEMY_PATROL,
	ENEMY_ATTACK,
	NUM_OF_ENEMY_STATE
};

class Enemy : public Actor {
public:
	Enemy(){}
	~Enemy(){}
	
public:
	static Enemy* SpawnPlayerWithPos( Vec2 pos );

public:
	void UpdateEnemy( float deltaSeconds );
	void RenderEnemy();

	// Accessor
	// Mutator
	void SetMap( Map* map );

	void CheckIsActive();
	void CheckIfReachGoal();

private:
	Map*
	bool m_isActive = false;
	EnemyAIState m_state;
	Vec2 m_goalPos = Vec2::ZERO;


public:
	float m_maxSpeed = 3.f;
	Rgba8 m_color = Rgba8::BLUE;
};

