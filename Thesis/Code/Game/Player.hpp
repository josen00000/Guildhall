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
	Player() = delete;
	~Player(){}
	explicit Player( int index, bool isBoss = false );
public:
	static Player* SpawnPlayerWithPos( Vec2 pos, int index );
	static Player* SpawnBossWithPos( Vec2 pos, int index );
public:
	void UpdatePlayer( float deltaSeconds, int playerIndex );
	void UpdateBoss( float deltaSeconds );
	void UpdatePlayerSpeed( float deltaSeconds );
	void HandleInput( float deltaSeconds, int playerIndex );
	void RenderPlayer();
	void RenderAimingLazer();
	void RenderBoss();
	void BossAttack1();
	void BossAttack2();
	void BossAttack3();
	virtual void Die() override;

	// Accessor
	bool IsContinousWalk() const { return m_isContinousWalk; }
	bool IsControlledByUser() const;
	bool GetIsStronger() const{ return m_isStronger; }
	int GetPlayerIndex();

	// Mutator
	void SetMap( Map* map );
	void SetInputControlState( InputControlState state );
	void SetDamage( float damage );
	void SetMaxSpeed( float maxSpeed );
	void SetIsStronger( bool isStronger );
	void DisableInputForSeconds( float seconds );
	void FakeDeadForSeconds( float seconds );
	bool Reborn( EventArgs& args );
	bool EnableInput( EventArgs& args );
	virtual void Shoot( float damage ) override;
private:
	void CheckInputMethod( int playerIndex );

	// AI function
	void CheckAIState();
	void FindGoalPatrolPoint( float deltaSeconds );
	void AIShoot( float damage );

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;
	bool m_isAbleInput		= true;
	bool m_isBoss			= false;
	bool m_isStronger		= false;
	Rgba8 m_togetherColor		= Rgba8::WHITE;

	InputControlState	m_inputState	= CONTROLLER_INPUT;
	PlayerAIState		m_aiState		= PLAYER_PATROL;

	int	m_index					= 0;

	float m_maxSpeed			= 3.f;
	float m_accelerate			= 1.5f;
	float m_patrolTotalTime		= 0.f;
	float m_activeDistThreshold = 10.f;
	float m_disableInputSeconds = 0.f;
	float m_damage				= 10.f;
	Vec2 m_patrolGoalPos	= Vec2::ZERO;
	Enemy* m_target	= nullptr;
};

