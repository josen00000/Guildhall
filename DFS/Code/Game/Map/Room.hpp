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


public:
	int			m_width		= 0;
	int			m_height	= 0;
	IntVec2		m_startCoords = IntVec2::ZERO;
	Map*		m_map		= nullptr;
	TileType	m_floorType = "";
	TileType	m_doorType = "";
};