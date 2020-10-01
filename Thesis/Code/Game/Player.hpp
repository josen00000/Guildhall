#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;

enum InputControlState {
	KEYBOARD_INPUT = 0,
	CONTROLLER_INPUT,
	NUM_OF_INPUT_STATE
};

class Player : public Actor {
public:
	Player();
	~Player(){}
	explicit Player( int index );
	
public:
	static Player* SpawnPlayerWithPos( Vec2 pos );

public:
	void UpdatePlayer( float deltaSeconds );
	void UpdatePlayerSpeed( float deltaSeconds );
	void HandleInput( float deltaSeconds );
	void RenderPlayer();

	// Accessor
	bool GetIsContinousWalk() const { return m_isContinousWalk; }

	// Mutator
	void SetMap( Map* map );
	void SetInputControlState( InputControlState state );
private:

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;

	InputControlState m_inputState = KEYBOARD_INPUT;
	int m_playerIndex	= 0;

	float m_maxSpeed = 3.f;
	float m_accelerate = 1.5f;
	Rgba8 m_color;

};

