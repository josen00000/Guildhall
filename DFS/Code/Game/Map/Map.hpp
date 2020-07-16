#pragma once
#include <string>
#include "Game/Game.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Map/Room.hpp"
#include "Game/Map/Maze.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class MapDefinition;
class Player;
class Actor;
class Timer;
class Item;
class Clock;
class SpriteAnimDefinition;
class SpriteSheet;
class Texture;

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

enum TileDirection : int {
	DIRECTION_NONE = 0,
	DIRECTION_UP,
	DIRECTION_DOWN,
	DIRECTION_LEFT,
	DIRECTION_RIGHT,
};

class Map {
public:
	Map(){}
	~Map();
	explicit Map( std::string name, MapDefinition* definition );
	static Map* CreateMap( std::string name, MapDefinition* definition );

public:
	void UpdateMap( float deltaSeconds );
	void RenderMap();

	// Accessor
	std::string GetName() const { return m_name; }
	int GetWidth() const { return m_width; }
	int GetHeight() const { return m_height; }
	Vec2 GetWorldPosWithTileCoords( IntVec2 tileCoords );

	Player* GetPlayer(){ return m_player; };
	RandomNumberGenerator* GetRNG()const { return m_rng; }

	TileDirection GetRevertTileDirection( TileDirection dirt );
	void GetNeighborsCoords( IntVec2 tileCoords, IntVec2* neighborCoords );

	Tile& GetTileWithTileCoords( const IntVec2& tileCoords );
	std::vector<Tile>& GetTiles() { return m_tiles; }
	std::vector<uint>& GetTileAttributes() { return m_tileAttributes; }
	TileTypes GetNeighborTypes( int tileIndex ) ;
	TileTypes GetNeighborTypes( IntVec2 tileCoords ) ;
	TileTypes GetSolidNeighborTypes( int tileIndex ) ;
	TileTypes GetSolidNeighborTypes( IntVec2 tileCoords ) ;

		// attrs
	bool IsTileRoom( IntVec2 tileCoords ) const;
	bool IsTileRoom( int tileIndex ) const;
	bool IsTileExit( IntVec2 tileCoords ) const;
	bool IsTileExit( int tileIndex ) const;
	bool IsTileStart( IntVec2 tileCoords ) const;
	bool IsTileStart( int tileIndex ) const;
	bool IsTileMapEdge( IntVec2 tileCoords ) const;
	bool IsTileMapEdge( int tileIndex ) const;
	bool IsTileSolid( IntVec2 tileCoords ) const;
	bool IsTileSolid( int tileIndex ) const;
	bool IsTileVisited( IntVec2 tileCoords ) const;
	bool IsTileVisited( int tileIndex ) const;
	bool IsTileDoor( IntVec2 tileCoords ) const;
	bool IsTileDoor( int tileIndex ) const;

	// Mutator
	void SetName( std::string name );
	void SetWidth( int width );
	void SetHeight( int height );
	void SetRoomNum( int roomNum );
	void AddRoomNum( int roomNum );
	void SetTileType( IntVec2 tileCoords, TileType type );
	void SetTileType( int tileX, int tileY, TileType type );
	void SetTileType( int tileIndex, TileType type );

	void SetTileConnectTo( IntVec2 tileCoords, TileDirection dirt ); 
	void SetTileNeighborsSolidWithDirt( IntVec2 tileCoords, TileDirection dirt );

	void AddRoom( Room* room );
	void AddMaze( Maze* maze );

		// attrs
	void SetTileRoom( IntVec2 tileCoords, bool isRoom );
	void SetTileRoom( int tileIndex, bool isRoom );
	void SetTileStart( IntVec2 tileCoords, bool isStart );
	void SetTileStart( int tileIndex, bool isStart );
	void SetTileExit( IntVec2 tileCoords, bool isExit );
	void SetTileExit( int tileIndex, bool isExit );
	void SetTileSolid( IntVec2 tileCoords, bool isSolid );
	void SetTileSolid( int tileIndex, bool isSolid );
	void SetTileVisited( IntVec2 tileCoords, bool isVisited );
	void SetTileVisited( int tileIndex, bool isVisited );
	void SetTileDoor( IntVec2 tileCoords, bool isDoor );
	void SetTileDoor( int tileIndex, bool isDoor );

	// Tiles
	IntVec2 GetTileCoordsWithTileIndex( int tileIndex ) const;
	IntVec2 GetRandomInsideTileCoords( ) const;
	IntVec2 GetRandomInsideWalkableTileCoords( ) const;
	int GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;

	bool IsTileCoordsValid( IntVec2 tileCoords ) const;
	bool IsTileOfTypeWithCoords( TileType type, IntVec2 tileCoords ) const;
	bool IsTileOfTypeWithIndex( TileType type, int tileIndex ) const;
	bool IsTileOfTypeInsideWithCoords( TileType type, IntVec2 tileCoords ) const;
	bool IsTileCoordsInside( const IntVec2& tileCoords ) const;

	// Fight
	bool GetIsFighting() const;
	const Actor* GetFightEnemy() const;

private:
	// Map Generation
	void CreateMapFromDefinition();
	void PopulateTiles();
	void InitializeTiles();
	void GenerateEdgeTiles( int edgeThick );
	void RunMapGenSteps();
	void PushTileVertices();
	void RandomGenerateStartAndExitCoords();
	void CheckTilesReachability();

	// Maze Generation
	void GenerateMazeTiles();
	void GenerateDoorTiles();
	void EliminateDeadEnds();
	void SetMazeNeighborEdge( IntVec2 tileCoords );
	bool IsTileCoordsDeadEnd( IntVec2 tileCoords );
	bool IsTileCoordsWalkableDeadEnd( IntVec2 tileCoords );
	bool IsEdgeConnectToMazeAndFloor( IntVec2 const& tileCoords, Room const* room, Maze const* maze );
	IntVec2 GetNeighborTileCoordsInDirection( IntVec2 tileCoords, TileDirection dirt );
	TileDirection GetValidNeighborDirection( IntVec2 tileCoords );

	// til attrs
	void SetTileAttrs( IntVec2 tileCoords, TileAttribute attr, bool state );
	void SetTileAttrs( int tileIndex, TileAttribute attr, bool state );
	bool isTileAttrs( IntVec2 tileCoords, TileAttribute attr ) const;
	bool isTileAttrs( int tileIndex, TileAttribute attr ) const;
	TileAttributeBitFlag GetTileAttrBitFlagWithAttr( TileAttribute attr ) const; 

	// Actor
	void CreateActors();
	void CreatePlayer();
	void CreateEnemies();
	void DeleteEnemy( Actor* enemy );

	// item
	void CreateKey();
	void CheckIfCollectKey();

	// Collision
	void CheckCollision();
	void CheckTileCollision();
	void CheckPlayerTileCollision();
	void CheckActorTileCollisionWithTileCoords( Actor* actor,  IntVec2 tileCoords );

	void CheckPlayerOnLava();

	// Fight
	void CheckFight();
	void StartFight( Actor* enemy );
	void UpdateFight( Actor* enemy, float deltaSeconds );
	void EndFight();
	void UpdateFightAnimation( float deltaSeconds );
	void RenderFightAnimation();

	// helper function
	void DebugDrawTiles();

public:
	int								m_mazeAndRoomCurrentLayer = 1;

private:
	bool							m_isFighting = false;
	
	int								m_width = 0;
	int								m_height = 0;
	int								m_roomNum = 0;

	float							m_mazeEdgePercentage = 0.9f;
	float							m_fightDeltaSecondsEachTurn = 0.5f;

	IntVec2							m_startCoords = IntVec2::ZERO;
	IntVec2							m_endCoords = IntVec2::ZERO;
	
	std::string						m_name = "";
	MapDefinition*					m_definition;
	std::vector<Tile>				m_tiles;
	std::vector<uint>				m_tileAttributes;
	std::vector<Vertex_PCU>			m_tilesVertices;
	std::vector<Vertex_PCU>			m_debugTilesVertices;
	std::vector<Vertex_PCU>			m_mazeVertices;
	std::vector<Room*>				m_rooms;
	std::vector<Maze*>				m_mazes;

	RandomNumberGenerator*			m_rng;

	TileType						m_mazeFloorType = "";
	TileType						m_mazeWallType	= "";

	// Actor
	Player*							m_player = nullptr;
	std::vector<Actor*>				m_enemies;

	// item
	Item*							m_key = nullptr;

	Timer*							m_fightTimer = nullptr;
	Actor*							m_fightEnemy = nullptr;

	// fight
	SpriteSheet*					m_fightSpriteSheet = nullptr;
	SpriteAnimDefinition*			m_fightAnimDef = nullptr;
	Texture*						m_fightTexture = nullptr;
	std::vector<Vertex_PCU>			m_fightVerts;
	AABB2							m_fightBox;
};