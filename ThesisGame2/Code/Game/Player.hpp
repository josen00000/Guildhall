#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Map;

class Player : public Actor {
public:
	Player() = delete;
	~Player(){}
	explicit Player( int index );
public:
	static Player* SpawnPlayerWithPos( Vec2 pos, int index );

public:
	void UpdatePlayer( float deltaSeconds, int playerIndex );
	void UpdatePlayerSpeed( float deltaSeconds );
	void HandleInput( float deltaSeconds, int playerIndex );
	void RenderPlayer( int controllerIndex = 0 );
	void RenderAimingLazer();
	void UpdateMarkersAndEdgeColor( );
	void RenderMarkers( int controllerIndex );
	virtual void TakeDamage( float damage ) override; 
	virtual void Die() override;

	// Accessor
	bool IsContinousWalk() const { return m_isContinousWalk; }
	int GetPlayerIndex();
	Rgba8 GetColor() const { return m_color; }

	// Mutator
	void SetMap( Map* map );
	void SetIndex( int index );
	void DisableInputForSeconds( float seconds );
	void FakeDeadForSeconds( float seconds );
	bool Reborn( EventArgs& args );
	bool EnableInput( EventArgs& args );

private:

public:
	bool m_isMoving			= false;
	bool m_isContinousWalk	= false;
	bool m_isAbleInput		= true;


	int	m_index					= 0;

	float m_maxSpeed			= 4.f;
	float m_accelerate			= 1.5f;
	float m_activeDistThreshold = 10.f;
	float m_disableInputSeconds = 0.f;
	std::vector<Vec2> m_markerPos;
	std::vector<Vertex_PCU> m_markerVertices;
};

