#pragma once
#include <string>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"

class TileDefinition;
class Rigidbody2D;

struct AABB2;

typedef	std::string TileType;
typedef Strings	TileTypes;

enum ConnectStateBit: uint {
	CONNECT_NONE_BIT	= 0,
	CONNECT_UP_BIT		= BIT_FLAG( 0 ),
	CONNECT_DOWN_BIT	= BIT_FLAG( 1 ),
	CONNECT_LEFT_BIT	= BIT_FLAG( 2 ),
	CONNECT_RIGHT_BIT	= BIT_FLAG( 3 ),
};

enum ConnectState {
	CONNECT_NONE,
	CONNECT_TO_UP,
	CONNECT_TO_DOWN,
	CONNECT_TO_LEFT,
	CONNECT_TO_RIGHT,
	NUM_OF_CONNECT
};

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
	TileType		GetTileType() const { return m_type; }

	bool		IsType( TileType type ) const;
	bool		IsConnectTo( ConnectState state );

	// Mutator
	void SetConnectTo( ConnectState state, bool isConnect );


	// Mutator
	void SetTileType( TileType type );

private:
	ConnectStateBit GetConnectStateBitFormConnectState( ConnectState state );
	/*IntVec2 GetTileCoords() const { return m_tileCoords; }*/

private:
	uint					m_connectState	= 0;
	IntVec2					m_tileCoords	= IntVec2( 0, 0 );
	TileType				m_type			= "";
	TileDefinition*			m_tileDef		= nullptr;
};