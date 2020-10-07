#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;
class Enemy;

enum InputControlState {
	KEYBOARD_INPUT = 0,
	CONTROLLER_INPUT,
	AI_INPUT,
	NUM_OF_INPUT_STATE
};

enum PlayerAIState {
	PLAYER_PATROL,
	PLAYER_ATTACK,
	NUM_OF_PLAYER_STATE
};

class Player : public Actor {
public:
	Player();
	~Player(){}
	
public:
	static Player* SpawnPlayerWithPos( Vec2 pos );

public:
	void UpdatePlayer( float deltaSeconds );
	void UpdatePlayerSpeed( float deltaSeconds );
	void HandleInput( float deltaSeconds );
	void RenderPlayer();

	// Accessor
	bool IsContinousWalk() const { return m_isContinousWalk; }
	bool IsControlledByUser() const;
	int	GetPlayerIndex() const { return m_playerIndex; }

	// Mutator
	void SetMap( Map* map );
	void SetPlayerIndex( int index );
	void SetInputControlState( InputControlState state );
private:
	void CheckInputMethod();

	// AI function
	void CheckAIState();
	void FindGoalPatrolPoint( float deltaSeconds );
	void AIShoot( float deltaSeconds );

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;

	InputControlState	m_inputState	= CONTROLLER_INPUT;
	PlayerAIState		m_aiState		= PLAYER_PATROL;

	int m_playerIndex	= 0;

	float m_maxSpeed			= 3.f;
	float m_accelerate			= 1.5f;
	float m_patrolTotalTime		= 0.f;
	float m_activeDistThreshold = 10.f;
	Vec2 m_patrolGoalPos	= Vec2::ZERO;
	Rgba8 m_color;

	Enemy* m_target	= nullptr;
};

