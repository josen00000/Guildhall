#include "Room.hpp"
#include "Game/Map/Map.hpp"


bool Room::IsTileOfRoomEdge( IntVec2 tileCoords ) const
{
	for( int i = 0; i < m_edgeTileCoords.size(); i++ ) {
		if( m_edgeTileCoords[i] == tileCoords ){ return true; }
	}
	return false;
}

bool Room::IsTileOfRoomFloor( IntVec2 tileCoords ) const
{
	for( int i = 0; i < m_floorTileCoords.size(); i++ ) {
		if( m_floorTileCoords[i] == tileCoords ) { 
			return true; 
		}
	}
	return false;
}

bool Room::IsTileOfRoom( IntVec2 tileCoords ) const
{
	return IsTileOfRoomFloor( tileCoords ) || IsTileOfRoomEdge( tileCoords );
}

IntVec2 Room::GetRandomEdgeTileCoord() const
{
	RandomNumberGenerator* mapRNG = m_map->GetRNG();
	int edgeTileIndex = mapRNG->RollRandomIntInRange( 0, (int)m_edgeTileCoords.size() - 1 );
	return m_edgeTileCoords[edgeTileIndex];
}

IntVec2 Room::GetRandomNearestEdgeTileCoord( IntVec2 tileCoords ) const
{
	IntVec2 endCoords = m_startCoords + IntVec2( m_width, m_height );
	IntVec2 result;
	RandomNumberGenerator* mapRNG = m_map->GetRNG();
	if( tileCoords.x < m_startCoords.x ) {
		result.x = m_startCoords.x;
	}
	else if( tileCoords.x > endCoords.x ) {
		result.x = endCoords.x;
	}
	else {
		result.x = mapRNG->RollRandomIntInRange( m_startCoords.x, endCoords.x );
	}

	if( tileCoords.y < m_startCoords.y ) {
		result.y = m_startCoords.y;
	}
	else if( tileCoords.y > endCoords.y ) {
		result.y = endCoords.y;
	}
	else {
		result.y = mapRNG->RollRandomIntInRange( m_startCoords.y, endCoords.y );
	}
	return result;
}

IntVec2 Room::GetRandomFloorTileCoord() const
{
	RandomNumberGenerator* mapRNG = m_map->GetRNG();
	int floorTileIndex = mapRNG->RollRandomIntInRange( 0, (int)m_floorTileCoords.size() - 1 );
	return m_floorTileCoords[floorTileIndex];
}

void Room::AddRoomEdgeTileCoords( IntVec2 tileCoords )
{
	m_edgeTileCoords.push_back( tileCoords );
}

void Room::AddRoomFloorTileCoords( IntVec2 tileCoords )
{
	m_floorTileCoords.push_back( tileCoords );
}

void Room::SetLayer( int layer )
{
	m_layer = layer;
}
