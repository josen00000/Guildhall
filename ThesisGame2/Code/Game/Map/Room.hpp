#pragma once
#include <vector>
#include "Game/Game.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

class Map;

class Room {
public:
	Room(){}
	~Room(){}

	int GetLayer() const { return m_layer; }
	bool IsTileOfRoomEdge( IntVec2 tileCoords ) const;
	bool IsTileOfRoomFloor( IntVec2 tileCoords ) const;
	bool IsTileOfRoom( IntVec2 tileCoords ) const;
	
	IntVec2 GetRandomEdgeTileCoord() const;
	IntVec2 GetRandomNearestEdgeTileCoord( IntVec2 tileCoords ) const;
	IntVec2 GetRandomFloorTileCoord() const;

	std::vector<IntVec2> GetEdgeTileCoords() { return m_edgeTileCoords; }
	std::vector<IntVec2> GetFloorTileCoords() { return m_floorTileCoords; }

	void AddRoomEdgeTileCoords( IntVec2 tileCoords ); 
	void AddRoomFloorTileCoords( IntVec2 tileCoords ); 
	void SetLayer( int layer );

public:
	int			m_width		= 0;
	int			m_height	= 0;
	int			m_layer		= 0;
	IntVec2		m_startCoords = IntVec2::ZERO;
	Map*		m_map		= nullptr;
	TileType	m_floorType = "";
	TileType	m_doorType = "";
	std::vector<IntVec2> m_edgeTileCoords;
	std::vector<IntVec2> m_floorTileCoords;
};