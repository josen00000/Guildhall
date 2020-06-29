#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;

enum MoveDirection {
	MOVE_EAST,
	MOVE_WEST,
	MOVE_NORTH,
	MOVE_SOUTH,
	IDLE
};

class Player : public Actor {
public:
	Player(){}
	~Player(){}
	explicit Player( ActorDefinition const& definition );

public:
	static Player* SpawnPlayerWithPos( ActorDefinition const& def, Vec2 pos );

public:
	void UpdatePlayer( float deltaSeconds );
	void UpdateAnimation( float deltaSeconds );
	void HandleInput( float deltaSeconds );
	void RenderPlayer();

	bool GetHasKey() const { return m_hasKey; }

	void SetHasKey( bool hasKey );

private:
	std::string GetMoveDirtInString( MoveDirection moveDirt ) const;

public:
	bool m_hasKey = false;
	float m_hp = 0.f;
	MoveDirection m_moveDirt = IDLE;
};

