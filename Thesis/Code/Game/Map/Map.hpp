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
class Boss;


enum TileAttributeBitFlag: uint {
	TILE_NON_ATTR_BIT		= 0,
	TILE_IS_START_BIT		= BIT_FLAG( 0 ),
	TILE_IS_EXIT_BIT		= BIT_FLAG( 1 ),
	TILE_IS_ROOM_BIT		= BIT_FLAG( 2 ),
	TILE_IS_SOLID_BIT		= BIT_FLAG( 3 ),
	TILE_IS_WALKABLE_BIT	= BIT_FLAG( 4 ),
	TILE_IS_SWIMMABLE_BIT	= BIT_FLAG( 5 ),
	TILE_IS_DOOR_BIT		= BIT_FLAG( 6 ),
};

enum TileAttribute {
	TILE_IS_START,	
	TILE_IS_EXIT,
	TILE_IS_ROOM,
	TILE_IS_SOLID,
	TILE_IS_WALKABLE,
	TILE_IS_SWIMMABLE,
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
	bool	IsTileOfTypeWithIndex( TileType  type, int index ) const;
	bool	IsTileCoordsInside( IntVec2 const& tileCoords ) const;
	bool	IsTileOfAttribute( IntVec2 const& tileCoords, TileAttribute attr ) const; 
	bool	IsTileOfAttribute( int tileIndex, TileAttribute attr ) const;
	bool	IsTileSolid( IntVec2 const& tileCoords ) const;
	bool	IsTileRoom( IntVec2 const& tileCoords ) const;
	bool	IsTileEdge( IntVec2 const& tileCoords ) const;
	bool	IsTileWalkable( IntVec2 const& tileCoords ) const;
	bool	IsTilesSameRoom( IntVec2 const& tileCoords1, IntVec2 const& tileCoords2 );
	bool	IsTilesSameRoomFloor( IntVec2 const& tileCoords1, IntVec2 const& tileCoords2 );
	bool	IsTileCoordsValid( IntVec2 const& tileCoords ) const;

	bool	IsPosNotSolid( Vec2 pos );

	int		GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;
	int		GetPlayerNum() const { return (int)m_players.size(); }
	int		GetEnemyNum() const { return (int)m_enemies.size(); }
	int		GetWidth() const { return m_width; }
	int		GetHeight() const { return m_height; }

	Vec2	GetPlayerPosWithIndex( int index ); 
	Vec2	GetCuePos( int index = -1 ) const;
	Vec2	GetRandomEnemyPos() const;
	std::string const& GetName() const { return m_name; }

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
	void SetTileTypeWithCoords( IntVec2 const& coords, TileType type );
	void SetTileTypeWithIndex( int index, TileType type );

	void SetTileOfAttribute( IntVec2 const& coords, TileAttribute attr, bool isTrue );
	void SetTileSolid( IntVec2 const& coords, bool isSolid );
	void SetTileWalkable( IntVec2 const& coords, bool isWalkable );
	void SetTileRoom( IntVec2 const& coords, bool isRoom );
	void SetStartcoords( IntVec2 const& coords );

	void AddRoom( Room* room );

	// Basic
	void UpdateMap( float deltaSeconds );
	void UpdateActors( float deltaSeconds );
	void UpdateProjectiles( float deltaSeconds );
	void UpdateItems( float deltaSeconds );
	void UpdateFirstLockedDoorsTiles();
	void UpdateSecondLockedDoorsTiles();
	void RenderMap();
	void RenderActors(); 
	void RenderProjectiles();
	void RenderItems();
	void EndFrame();
	void GenerateMap();

	// Actor
	void CreatePlayer();
	void DestroyPlayerWithIndex( int index );
	void DestroyAllPlayers();
	void ShiftPlayer();
	void SpawnNewEnemy( Vec2 startPos );

	// Projectile
	void SpawnNewProjectile( ActorType type, Vec2 startPos, Vec2 movingDirt, Rgba8 color );
	void SpawnNewProjectileWithDamage( ActorType type, Vec2 startPos, Vec2 movingDirt, Rgba8 color, float damage );

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
	void RunMapGenSteps();
	void GeneratePaths();
	void CreatePathBetweenCoords( IntVec2 coord1, IntVec2 coord2 );
	void InitializeMapDifferentTiles();
	void CheckNeedGenerateEnemies();
	void CheckNeedGenerateBoss();
	void CheckPlayerStates();
	bool GenerateEnemies( EventArgs& args );
	void GenerateBoss();

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
	bool							m_isFirstDoorLocked = true;
	bool							m_isSecondDoorLocked = true;
	bool							m_isEnemiesGenerated = false;
	bool							m_isBossGenerated = false;
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
	std::vector<Enemy*>				m_enemies;
	std::vector<Projectile*>		m_projectiles;

	Player*							m_boss = nullptr;
	// Item
	std::vector<Item*>				m_items;
	Item*							m_activeItem = nullptr;
	std::vector<IntVec2>			m_triggerTiles;
	std::vector<IntVec2>			m_doorTiles;
};