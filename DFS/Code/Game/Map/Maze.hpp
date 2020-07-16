#pragma once
#include <vector>
#include <stack>
#include "Game/Game.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/IntVec2.hpp"

class Map;

class Maze {
public:
	Maze(){}
	~Maze(){}
	explicit Maze( Map* map );

	bool isTileOfMaze( IntVec2 tileCoords ) const;
	int Getlayer() const { return m_layer; }
	IntVec2 GetRandomTileCoords() const;

	void AddMazeTileCoords( IntVec2 tileCoords );
	void SetLayer( int layer );

public:
	int				m_layer		= 0;
	Map*			m_map		= nullptr;
	std::vector<IntVec2> m_tileCoords;
	std::stack<IntVec2> m_deadEndtileCoords;
};