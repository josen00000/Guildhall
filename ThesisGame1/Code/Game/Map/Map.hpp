#pragma once
#include <string>
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Map/Room.hpp"
#include "Game/Map/Maze.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class MapDefinition;
class Player;
class Actor;
class Enemy;
class Timer;
class Item;
class Clock;
class SpriteAnimDefinition;
class SpriteSheet;
class Texture;
class Projectile;


typedef std::pair<Vec2, float> explosionPosAndTime;

enum TileAttributeBitFlag: uint {
	TILE_NON_ATTR_BIT		= 0,
	TILE_IS_START_BIT		= BIT_FLAG( 0 ),
	TILE_IS_EXIT_BIT		= BIT_FLAG( 1 ),
	TILE_IS_ROOM_BIT		= BIT_FLAG( 2 ),
	TILE_IS_SOLID_BIT		= BIT_FLAG( 3 ),
	TILE_IS_VISITED_BIT		= BIT_FLAG( 4 ),
	TILE_IS_DOOR_BIT		= BIT_FLAG( 5 ),
};

enum TileAttribute {
	TILE_IS_START,	
	TILE_IS_EXIT,
	TILE_IS_ROOM,
	TILE_IS_SOLID,
	TILE_IS_VISITED,
	TILE_IS_DOOR,
	NUM_TILE_ATTRS
};

enum tileDirection
{
	TILE_UP,
	TILE_DOWN,
	TILE_LEFT,
	TILE_RIGHT
};

enum ePlayerTurnState
{
	PLAYER_1_TURN,
	PLAYER_2_TURN,
};

enum eDynamicSplitState {
	NO_STATE,
	AIMS_FIRST,
	HEALTH_BASE,
};

class Map {
public:
	Map(){}
	~Map();
	explicit Map( std::string name, MapDefinition* definition );
	static Map* CreateMap( std::string name, MapDefinition* definition );

	// Accessor
		// Tile
	bool	IsTileOfTypeInsideWithCoords( TileType type, IntVec2 tileCoords ) const;
	bool	IsTileOfTypeWithIndex( TileType type, int index ) const;
	bool	IsTileCoordsInside( IntVec2 tileCoords ) const;
	bool	IsTileOfAttribute( IntVec2 tileCoords, TileAttribute attr ) const; 
	bool	IsTileOfAttribute( int tileIndex, TileAttribute attr ) const;
	bool	IsTileSolid( IntVec2 tileCoords ) const;
	bool	IsTileRoom( IntVec2 tileCoords ) const;
	bool	IsTileEdge( IntVec2 tileCoords ) const;
	bool	IsTileVisited( IntVec2 tileCoords ) const;
	bool	IsTileCoordsValid( IntVec2 tileCoords ) const;

	bool	IsPosNotSolid( Vec2 pos );

	int		GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;
	int		GetPlayerNum() const { return (int)m_players.size(); }
	
	Vec2	GetAimPos() const { return m_aimPos; }
	Vec2	GetPlayerPosWithIndex( int index ); 
	Vec2	GetCuePos(){ return Vec2::ZERO; }
	AABB2	GetCurrentAimingCameraAsBox();

	Player*	GetPlayerWithIndex( int index );
	
	std::vector<Player*>& GetPlayers();

	IntVec2 GetTileCoordsWithTileIndex( int index ) const;
	IntVec2 GetRandomInsideTileCoords() const;
	IntVec2 GetRandomInsideNotSolidTileCoords() const;
	IntVec2 GetRandomInsideCameraNotSolidTileCoords( Camera* camera ) const;
	
	std::vector<Tile>& GetTiles(){ return m_tiles; }
	const Tile&	GetTileWithTileCoords( IntVec2 coords );

	RandomNumberGenerator* GetRNG(){ return m_rng; }
	int GetKeyboardPlayerIndex() const { return m_keyboardPlayerIndex; }

	// Mutator
	void SetAimPos( Vec2 pos );
	void SetTileTypeWithCoords( IntVec2 coords, TileType type );
	void SetTileTypeWithIndex( int index, TileType type );

	void SetTileOfAttribute( IntVec2 coords, TileAttribute attr, bool isTrue );
	void SetTileSolid( IntVec2 coords, bool isSolid );
	void SetTileRoom( IntVec2 coords, bool isRoom );
	void SetAimMoveDirt( Vec2 moveDirt );

	// Basic
	void UpdateMap( float deltaSeconds );
	void UpdateActors( float deltaSeconds );
	void UpdateItems( float deltaSeconds );
	void RenderMap();
	void RenderActors(); 
	void RenderAim();
	void EndFrame();

	// Actor
	void CreatePlayer();
	void CreatePlayerAt( IntVec2 pos );
	void DestroyPlayerWithIndex( int index );

	void ExplodeAtAim();
	bool FinishExplosion( EventArgs& args );

	// Collision
	void CheckCollision();
	void CheckActorsCollision();
	void CheckCollisionBetweenActors( Actor* actorA, Actor* actorB );
	void CheckCollisionBetweenActorAndTiles( Actor* actor );
	void CheckCollisionBetweenActorAndTile( Actor* actor, IntVec2 tileCoords );
	void CheckTilesCollision();
	void CheckCollisionBetweenProjectileAndActors( Projectile* projectile );
	void CheckCollisionBetweenProjectileAndTiles( Projectile* projectile ); 

	void CheckIfTurnChange();

	// Garbage management
	void ManageGarbage();

private:
	// Generate Map
	void LoadMapDefinitions();
	void PopulateTiles();
	void InitializeTiles();
	void GenerateEdges();
	
	void GeneratePlayers();
	void GenerateTitleVertices();

	// private update
	void UpdateDebugInfo();
	void UpdateMouse();
	void UpdateTurn();
	void updateAimPos( float deltaSeconds );
	void UpdateExplosions( float deltaSeconds );

	// private Render
	void RenderDebugInfo();
	void RenderDebugTile( int tileIndex );
	void RenderExplosions();

	// private Accessor
	TileAttributeBitFlag GetTileAttrBitFlagWithTileAttr( TileAttribute attr ) const;

private:
	int								m_width					= 0;
	int								m_height				= 0;
	int								m_roomNum				= 0;
	int								m_debugTileIndex		= 0;
	int								m_keyboardPlayerIndex	= 0;

	float							m_explosionRadius		= 3.f;
	float							m_explosionDuration		= 2.f;

	eDynamicSplitState				m_dynamicSplitState = AIMS_FIRST;
	ePlayerTurnState				m_turn				= PLAYER_1_TURN;
	bool							m_isGeneratingMap	= true;

	IntVec2							m_startCoords		= IntVec2::ZERO;
	IntVec2							m_endCoords			= IntVec2::ZERO;

	Vec2							m_mousePosInWorld	= Vec2::ZERO;
	Vec2							m_aimPos			= Vec2::ZERO;
	Vec2							m_aimMoveDirt		= Vec2::ZERO;
	
	std::string						m_name;

	MapDefinition*					m_definition = nullptr;
	RandomNumberGenerator*			m_rng;

	std::vector<Tile>					m_tiles;
	std::vector<uint>					m_tileAttributes;
	std::vector<Vertex_PCU>				m_tilesVertices;
	std::vector<Vertex_PCU>				m_explosionVertices;
	std::vector<Vertex_PCU>				m_titleVertices;
	std::vector<explosionPosAndTime>	m_explosions;

	// Actor
	std::vector<Player*>			m_players;

};