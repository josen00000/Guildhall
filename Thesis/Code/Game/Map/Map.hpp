#pragma once
#include <string>
#include "Game/Game.hpp"
#include "Game/Map/Tile.hpp"
#include "Game/Map/Room.hpp"
#include "Game/Map/Maze.hpp"
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

	IntVec2 GetTileCoordsWithTileIndex( int index ) const;
	IntVec2 GetRandomInsideTileCoords() const;
	IntVec2 GetRandomInsideNotSolidTileCoords() const;
	int		GetTileIndexWithTileCoords( IntVec2 tileCoords ) const;

	RandomNumberGenerator* GetRNG(){ return m_rng; }

	const Tile&	GetTileWithTileCoords( IntVec2 coords );
	std::vector<Tile>& GetTiles(){ return m_tiles; }


	// Mutator
	void SetTileTypeWithCoords( IntVec2 coords, TileType type );
	void SetTileTypeWithIndex( int index, TileType type );

	void SetTileOfAttribute( IntVec2 coords, TileAttribute attr, bool isTrue );
	void SetTileSolid( IntVec2 coords, bool isSolid );
	void SetTileRoom( IntVec2 coords, bool isRoom );

	void AddRoom( Room* room );

	// Basic
	void UpdateMap( float deltaSeconds );
	void RenderMap();

private:
	// Generate Map
	void LoadMapDefinitions();
	void PopulateTiles();
	void InitializeTiles();
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

	bool							m_isGeneratingMap = true;

	IntVec2							m_startCoords = IntVec2::ZERO;
	IntVec2							m_endCoords = IntVec2::ZERO;

	Vec2							m_mousePosInWorld = Vec2::ZERO;
	
	std::string						m_name;

	MapDefinition*					m_definition = nullptr;
	std::vector<Tile>				m_tiles;
	std::vector<uint>				m_tileAttributes;
	std::vector<Vertex_PCU>			m_tilesVertices;
	std::vector<Room*>				m_rooms;
	std::vector<Maze*>				m_mazes;

	RandomNumberGenerator*			m_rng;

	int								m_debugTileIndex = 0;

	// Actor
	Player*							m_player = nullptr;
	std::vector<Actor*>				m_enemies;
};