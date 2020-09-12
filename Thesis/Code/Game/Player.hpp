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
	void HandleInput( float deltaSeconds );
	void RenderPlayer();

	// Accessor

	// Mutator
	void SetMap( Map* map );
private:

public:
	Rgba8 m_color;
};

