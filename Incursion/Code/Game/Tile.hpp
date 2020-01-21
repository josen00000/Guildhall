#pragma once
#include<engine/Math/vec2.hpp>
#include<engine/Math/IntVec2.hpp>
#include<vector>
#include<Engine/Core/Vertex_PCU.hpp>
#include<Engine/Core/Rgba8.hpp>


struct AABB2;
class Entity;


enum TileType {
	TILE_TYPE_NULL = -1,
	TILE_TYPE_GRASS,
	TILE_TYPE_STONE,
	TILE_TYPE_MUD,
	TILE_TYPE_BRICK,
	TILE_TYPE_LAVA,
	TILE_TYPE_WATER,
	NUM_TILE_TYPES
};


class Tile {
public:
	Tile(int tileX,int tileY);
	Tile(int tileX, int tileY, TileType tileType);
	~Tile(){}
	Tile(const Tile& copyFrom);
	//explicit
	explicit Tile( IntVec2& tileCoords);
	void Update(float deltaSeconds);
	void Render()const;
	void DebugRender()const;
	AABB2 GetBounds() const;
	void SetTileType(const TileType tileType);

public:	
	TileType m_type = TILE_TYPE_GRASS;
	IntVec2 m_tileCoords = IntVec2(0,0);
};