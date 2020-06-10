#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"

class TileDefinition;
class Rigidbody2D;

struct AABB2;

typedef	std::string TileType;

struct Tile {
public:
	Tile(){}
	~Tile(){}
	explicit Tile( int tileX, int tileY, TileType type );
	explicit Tile( IntVec2 tileCoords, TileType type );

	// Accessor
	AABB2			GetTileBounds() const;
	Vec2			GetWorldPos() const;
	TileDefinition* GetTileDef() const { return m_tileDef; }

	// Mutator
	void SetTileType( TileType type );
	void SetRigidbody2D( Rigidbody2D* rb2D );

public:
	IntVec2 GetTileCoords() const { return m_tileCoords; }

private:
	IntVec2					m_tileCoords	= IntVec2( 0, 0 );
	TileType				m_type			= "";
	TileDefinition*			m_tileDef		= nullptr;
	Rigidbody2D*			m_rb			= nullptr;
};