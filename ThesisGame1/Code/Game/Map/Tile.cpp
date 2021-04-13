#include "Tile.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/RigidBody2D.hpp"


Tile::Tile( IntVec2 tileCoords, TileType type )
	:m_tileCoords(tileCoords)
	,m_type(type)
{
	m_tileDef = TileDefinition::s_definitions[type];
}

Tile::Tile( int tileX, int tileY ,TileType type )
{
	m_tileCoords.x = tileX;
	m_tileCoords.y = tileY;
	m_type = type;
	m_tileDef = TileDefinition::s_definitions[type];
}

AABB2 Tile::GetTileBounds() const
{
	Vec2 mins( (float)m_tileCoords.x, (float)m_tileCoords.y );
	Vec2 maxs(( mins.x + 1.f ), ( mins.y + 1.f));
	return AABB2( mins, maxs ); 
}

Vec2 Tile::GetWorldPos() const
{
	Vec2 tileCoords_v2 = Vec2( (float)m_tileCoords.x, (float)m_tileCoords.y );
	return tileCoords_v2 + Vec2( 0.5f );
}

bool Tile::IsType( TileType type ) const
{
	return ( m_type.compare( type ) == 0 );
}

bool Tile::IsConnectTo( ConnectState state )
{
	ConnectStateBit connectBit = GetConnectStateBitFormConnectState( state );
	return ( m_connectState & connectBit ) == connectBit;
}

void Tile::SetConnectTo( ConnectState state, bool isConnect )
{
	if( isConnect ){
		switch( state )
		{
		case CONNECT_TO_LEFT:
			m_connectState = m_connectState | CONNECT_LEFT_BIT;
			break;
		case CONNECT_TO_RIGHT:
			m_connectState = m_connectState | CONNECT_RIGHT_BIT;
			break;
		case CONNECT_TO_UP:
			m_connectState = m_connectState | CONNECT_UP_BIT;
			break;
		case CONNECT_TO_DOWN:
			m_connectState = m_connectState | CONNECT_DOWN_BIT;
			break;
		}
	}
	else {
		switch( state )
		{
		case CONNECT_TO_LEFT:
			m_connectState = m_connectState & ~CONNECT_LEFT_BIT;
			break;
		case CONNECT_TO_RIGHT:
			m_connectState = m_connectState & ~CONNECT_RIGHT_BIT;
			break;
		case CONNECT_TO_UP:
			m_connectState = m_connectState & ~CONNECT_UP_BIT;
			break;
		case CONNECT_TO_DOWN:
			m_connectState = m_connectState & ~CONNECT_DOWN_BIT;
			break;
		}
	}
}

void Tile::SetTileType( TileType type )
{
	m_type = type;
	m_tileDef = TileDefinition::s_definitions[m_type];
}

ConnectStateBit Tile::GetConnectStateBitFormConnectState( ConnectState state )
{
	switch( state )
	{
	case CONNECT_NONE:			return CONNECT_NONE_BIT;
	case CONNECT_TO_LEFT:		return CONNECT_LEFT_BIT;
	case CONNECT_TO_RIGHT:		return CONNECT_RIGHT_BIT;
	case CONNECT_TO_UP:			return CONNECT_UP_BIT;
	case CONNECT_TO_DOWN:		return CONNECT_DOWN_BIT;
	}
	return CONNECT_NONE_BIT;
}
