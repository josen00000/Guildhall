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

	// Accessor
	bool GetHasKey() const { return m_hasKey; }
	bool GetDisableInput() const { return m_disableInput; }
	bool GetDisableMove() const { return m_disableMove; }

	// Mutator
	void SetHasKey( bool hasKey );
	void SetDisableInput( bool disableInput );
	void SetDisableMove( bool disableMove );
	void SetIsOnLava( bool onLava );
	
private:
	std::string GetMoveDirtInString( MoveDirection moveDirt ) const;
	void PlayWalkSound( float deltaSeconds );

public:
	bool m_hasKey		= false;
	bool m_disableInput = false;
	bool m_disableMove	= false;
	bool m_isOnLava		= false;
	MoveDirection m_moveDirt = IDLE;
};

