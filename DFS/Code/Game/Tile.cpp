#include "Tile.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/RigidBody2D.hpp"


Tile::Tile( IntVec2 tileCoords, TileType type )
	:m_tileCoords(tileCoords)
	,m_type(type)
{
}

Tile::Tile( int tileX, int tileY ,TileType type )
{
	m_tileCoords.x = tileX;
	m_tileCoords.y = tileY;
	m_type = type;
}

AABB2 Tile::GetTileBounds() const
{
	Vec2 mins( (float)m_tileCoords.x, (float)m_tileCoords.y );
	Vec2 maxs(( mins.x + 1.f ), ( mins.y + 1.f));
	return AABB2( mins, maxs ); 
}

Vec2 Tile::GetWorldPos() const
{
	Vec2 tileCoords_v2 = Vec2( m_tileCoords.x, m_tileCoords.y );
	return tileCoords_v2 + Vec2( 0.5f );
}

void Tile::SetTileType( TileType type )
{
	m_type = type;
}

void Tile::SetRigidbody2D( Rigidbody2D* rb2D )
{
	m_rb = rb2D;
}
