#include "GenRooms.hpp"
#include "Game/Map/map.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


GenRooms::GenRooms( const XmlElement& roomElement )
	:MapGenStep(roomElement)
{
	m_roomsNumRange		= ParseXmlAttribute( roomElement, "roomsNum", m_roomsNumRange );
	m_widthRange		= ParseXmlAttribute( roomElement, "width", m_widthRange );
	m_heightRange		= ParseXmlAttribute( roomElement, "height", m_heightRange );
	m_floorType			= ParseXmlAttribute( roomElement, "floorType", m_floorType );
	m_wallType			= ParseXmlAttribute( roomElement, "wallType", m_wallType );
	m_doorType			= ParseXmlAttribute( roomElement, "doorType", m_doorType );
}

void GenRooms::RunStepOnce( Map* map ) const
{
	RandomNumberGenerator* mapRNG = map->GetRNG();
	int roomNum = m_roomsNumRange.GetRandomInRange( *mapRNG );
	for( int i = 0; i < roomNum; i++ ) {
		CreateRoom( map, *mapRNG );
	}
	map->AddRoomNum( roomNum );
}

void GenRooms::CreateRoom( Map* map, RandomNumberGenerator rng ) const
{
	bool isStartTileValid = false;
	int roomWidth = m_widthRange.GetRandomInRange( rng );
	int roomHeight = m_widthRange.GetRandomInRange( rng );
	IntVec2 startTileCoords = map->GetRandomInsideTileCoords();
	IntVec2 endTileCoords;
	
	while( !isStartTileValid ) {
		isStartTileValid = true;
		endTileCoords = startTileCoords + IntVec2( roomWidth, roomHeight );

		if( !map->IsTileCoordsInside( startTileCoords ) ) {
			isStartTileValid = false;
			startTileCoords = map->GetRandomInsideTileCoords();
			continue;
		}
		if( !map->IsTileCoordsInside( endTileCoords ) ) {
			isStartTileValid = false;
			startTileCoords = map->GetRandomInsideTileCoords();
			continue;
		}
		
		// check overlap
		if ( map->IsTileRoom( startTileCoords )) {
			isStartTileValid = false;
		}
		if ( map->IsTileRoom( endTileCoords )) {
			isStartTileValid = false;
		}
		if ( map->IsTileRoom( IntVec2( startTileCoords.x, endTileCoords.y ))) {
			isStartTileValid = false;
		}
		if ( map->IsTileRoom( IntVec2( endTileCoords.x,startTileCoords.y ))) {
			isStartTileValid = false;
		}

		if( !isStartTileValid ) {
			startTileCoords = map->GetRandomInsideTileCoords();
		}
	}

	// room floor
	for( int tileX = startTileCoords.x; tileX <= endTileCoords.x; tileX++ ) {
		for( int tileY = startTileCoords.y; tileY <= endTileCoords.y; tileY++ ) {
			map->SetTileType( tileX, tileY, m_floorType );
			map->SetTileRoom( IntVec2( tileX, tileY), true );
		}
	}

	// room edge
	for( int tileX = startTileCoords.x; tileX <= endTileCoords.x; tileX++ ) {
		map->SetTileType( tileX, startTileCoords.y, m_wallType );
		map->SetTileRoom( IntVec2( tileX, startTileCoords.y ), true );
		map->SetTileEdge( IntVec2( tileX, startTileCoords.y ), true );
		map->SetTileSolid( IntVec2( tileX, startTileCoords.y ), true );
		
		map->SetTileType( tileX, endTileCoords.y, m_wallType );
		map->SetTileRoom( IntVec2( tileX, endTileCoords.y ), true );
		map->SetTileEdge( IntVec2( tileX, endTileCoords.y ), true );
		map->SetTileSolid( IntVec2( tileX, endTileCoords.y ), true );
	}

	for( int tileY = startTileCoords.y; tileY <= endTileCoords.y; tileY++ ) {
		map->SetTileType( startTileCoords.x, tileY, m_wallType );
		map->SetTileRoom( IntVec2( startTileCoords.x, tileY ), true );
		map->SetTileEdge( IntVec2( startTileCoords.x, tileY ), true );
		map->SetTileSolid( IntVec2( startTileCoords.x, tileY ), true );

		map->SetTileType( endTileCoords.x, tileY, m_wallType );
		map->SetTileRoom( IntVec2( endTileCoords.x, tileY ), true );
		map->SetTileEdge( IntVec2( endTileCoords.x, tileY ), true );
		map->SetTileSolid( IntVec2( endTileCoords.x, tileY ), true );
	}
	
	Room tempRoom = Room();
	tempRoom.m_width = roomWidth;
	tempRoom.m_height = roomHeight;
	tempRoom.m_floorType = m_floorType;
	tempRoom.m_map = map;
	tempRoom.m_startCoords = startTileCoords;
	tempRoom.m_doorType = m_doorType;
	map->AddRoom( tempRoom );

// 	room door
// 		IntVec2 doorCoords = IntVec2();
// 		int randAxis = rng.RollRandomIntInRange( 0, 1 );
// 		if( randAxis == 0 ) {
// 			doorCoords.x = rng.RollRandomIntInRange( startTileCoords.x, endTileCoords.x );
// 			int randY = rng.RollRandomIntInRange( 0, 1 );
// 			if( randY == 0 ) {
// 				doorCoords.y = startTileCoords.y;
// 			}
// 			else {
// 				doorCoords.y = endTileCoords.y;
// 			}
// 		}
// 		else {
// 			doorCoords.y = rng.RollRandomIntInRange( startTileCoords.y, endTileCoords.y );
// 			int randX = rng.RollRandomIntInRange( 0, 1 );
// 			if( randX == 0 ) {
// 				doorCoords.x = startTileCoords.x;
// 			}
// 			else{
// 				doorCoords.x = endTileCoords.x;
// 			}
// 		}
// 		map->SetTileType( doorCoords, m_doorType );
}
