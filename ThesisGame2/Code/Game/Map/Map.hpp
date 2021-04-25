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
	bool	IsTilesSameRoom( IntVec2 tileCoords1, IntVec2 tileCoords2 );
	bool	IsTilesSameRoomFloor( IntVec2 tileCoords1, IntVec2 tileCoords2 );
	bool	IsTileCoordsValid( IntVec2 tileCoords ) const;

	bool	IsPosNotSolid( Vec2 pos );

	int		GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;
	int		GetPlayerNum() const { return (int)m_players.size(); }

	Vec2	GetPlayerPosWithIndex( int index ); 
	Vec2	GetCuePos() const;

	Player*	GetPlayerWithIndex( int index );
	
	std::vector<Enemy*>& GetEnemies();
	std::vector<Player*>& GetPlayers();

	IntVec2 GetTileCoordsWithTileIndex( int index ) const;
	IntVec2 GetRandomInsideTileCoords() const;
	IntVec2 GetRandomInsideNotSolidTileCoords() const;
	IntVec2 GetRandomInsideCameraNotSolidTileCoords( Camera* camera ) const;
	


	std::vector<Tile>& GetTiles(){ return m_tiles; }
	const Tile&	GetTileWithTileCoords( IntVec2 coords );

	RandomNumberGenerator* GetRNG(){ return m_rng; }
		// Actor
	int GetKeyboardPlayerIndex() const { return m_keyboardPlayerIndex; }

	// Mutator
	void SetTileTypeWithCoords( IntVec2 coords, TileType type );
	void SetTileTypeWithIndex( int index, TileType type );

	void SetTileOfAttribute( IntVec2 coords, TileAttribute attr, bool isTrue );
	void SetTileSolid( IntVec2 coords, bool isSolid );
	void SetTileRoom( IntVec2 coords, bool isRoom );

	void AddRoom( Room* room );

	// Basic
	void UpdateMap( float deltaSeconds );
	void UpdateActors( float deltaSeconds );
	void UpdateProjectiles( float deltaSeconds );
	void UpdateItems( float deltaSeconds );
	void RenderMap( int controllerIndex = 0 );
	void RenderActors( int controllerIndex = 0 ); 
	void RenderProjectiles();
	void RenderItems();
	void EndFrame();
	void GenerateMap();

	// Actor
	void CreatePlayer();
	void DestroyPlayerWithIndex( int index );
	void DestroyAllPlayers();

	// Projectile
	void SpawnNewProjectile( int playerIndex, Vec2 startPos, Vec2 movingDirt, Rgba8 color );

	// Item
	void SpawnNewItem( Vec2 startPos );

	// Collision
	void CheckCollision();
	void CheckActorsCollision();
	void CheckCollisionBetweenActors( Actor* actorA, Actor* actorB );
	void CheckCollisionBetweenActorAndTiles( Actor* actor );
	void CheckCollisionBetweenActorAndTile( Actor* actor, IntVec2 tileCoords );
	void CheckTilesCollision();
	void CheckBulletsCollision();
	void CheckCollisionBetweenProjectileAndActors( Projectile* projectile );
	void CheckCollisionBetweenProjectileAndTiles( Projectile* projectile ); 

	// Garbage management
	void ManageGarbage();

private:
	// Generate Map
	void LoadMapDefinitions();
	void PopulateTiles();
	void InitializeTiles();
	void GenerateEdges();
	void GenerateRooms();
	void GeneratePaths();
	void CreatePathBetweenCoords( IntVec2 coord1, IntVec2 coord2 );
	

	// private update
	void UpdateDebugInfo();
	void UpdateMouse();

	// private Render
	void RenderDebugInfo();
	void RenderDebugTile( int tileIndex );

	// private Accessor
	TileAttributeBitFlag GetTileAttrBitFlagWithTileAttr( TileAttribute attr ) const;

private:
	int								m_width = 0;
	int								m_height = 0;
	int								m_roomNum = 0;

	IntVec2							m_startCoords = IntVec2::ZERO;
	IntVec2							m_endCoords = IntVec2::ZERO;

	Vec2							m_mousePosInWorld = Vec2::ZERO;
	
	std::string						m_name;

	MapDefinition*					m_definition = nullptr;
	std::vector<Tile>				m_tiles;
	std::vector<uint>				m_tileAttributes;
	std::vector<Vertex_PCU>			m_tilesVertices;
	std::vector<Room*>				m_rooms;

	RandomNumberGenerator*			m_rng;

	int								m_debugTileIndex = 0;
	int								m_keyboardPlayerIndex = 0;

	// Actor
	std::vector<Player*>			m_players;
	std::vector<Projectile*>		m_projectiles;

	// Item
	std::vector<Item*>				m_items;
	Item*							m_activeItem = nullptr;
};