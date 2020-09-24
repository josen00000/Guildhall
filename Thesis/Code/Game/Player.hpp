#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;

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
	bool GetIsContinousWalk() const { return m_isContinousWalk; }

	// Mutator
	void SetMap( Map* map );
private:

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;
	float m_maxSpeed = 3.f;
	float m_accelerate = 1.5f;
	Rgba8 m_color;

};

