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
	void UpdatePlayer( float deltaSeconds, int playerIndex );
	void UpdatePlayerSpeed( float deltaSeconds );
	void HandleInput( float deltaSeconds, int playerIndex );
	void RenderPlayer();
	virtual void Die() override;

	// Accessor
	bool IsContinousWalk() const { return m_isContinousWalk; }
	bool IsControlledByUser() const;
	int GetPlayerIndex();

	// Mutator
	void SetMap( Map* map );
	void SetInputControlState( InputControlState state );
	void DisableInputForSeconds( float seconds );
	void FakeDeadForSeconds( float seconds );
	bool Reborn( EventArgs& args );
	bool EnableInput( EventArgs& args );

private:
	void CheckInputMethod( int playerIndex );

	// AI function
	void CheckAIState();
	void FindGoalPatrolPoint( float deltaSeconds );
	void AIShoot( float deltaSeconds );

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;
	bool m_isAbleInput		= true;

	InputControlState	m_inputState	= CONTROLLER_INPUT;
	PlayerAIState		m_aiState		= PLAYER_PATROL;


	float m_maxSpeed			= 3.f;
	float m_accelerate			= 1.5f;
	float m_patrolTotalTime		= 0.f;
	float m_activeDistThreshold = 10.f;
	float m_disableInputSeconds = 0.f;
	Vec2 m_patrolGoalPos	= Vec2::ZERO;
	Rgba8 m_color;

	Enemy* m_target	= nullptr;
};

