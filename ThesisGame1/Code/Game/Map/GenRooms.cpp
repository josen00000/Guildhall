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
		//map->m_mazeAndRoomCurrentLayer++;
	}
	//map->AddRoomNum( roomNum );
}

void GenRooms::CreateRoom( Map* map, RandomNumberGenerator rng ) const
{
	bool isStartTileValid = false;
	int roomWidth = m_widthRange.GetRandomInRange( rng );
	int roomHeight = m_widthRange.GetRandomInRange( rng );
	IntVec2 startTileCoords = map->GetRandomInsideTileCoords();
	IntVec2 endTileCoords;

	Room* tempRoom = new Room();
	tempRoom->m_width = roomWidth;
	tempRoom->m_height = roomHeight;
	tempRoom->m_edgeTileCoords.reserve( roomWidth * 2 + roomHeight * 2 );
	tempRoom->m_floorTileCoords.reserve( roomWidth * roomHeight );
	
	while( !isStartTileValid ) {
		isStartTileValid = true;
		endTileCoords = startTileCoords + IntVec2( roomWidth, roomHeight );

		if( !map->IsTileCoordsInside( startTileCoords ) ) {
			isStartTileValid = false;
			startTileCoords = map->GetRandomInsideNotSolidTileCoords();
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
 	for( int tileX = startTileCoords.x + 1; tileX <= endTileCoords.x - 1; tileX++ ) {
 		for( int tileY = startTileCoords.y + 1; tileY <= endTileCoords.y - 1; tileY++ ) {
 			IntVec2 tileCoords = IntVec2( tileX, tileY );
 			map->SetTileTypeWithCoords( tileCoords, m_floorType );
 			map->SetTileRoom( tileCoords, true );
 			tempRoom->AddRoomFloorTileCoords( tileCoords );
 		}
 	}

	// room edge
 	for( int tileX = startTileCoords.x; tileX <= endTileCoords.x; tileX++ ) {
 		IntVec2 startYCoords = IntVec2( tileX, startTileCoords.y );
 		IntVec2 endYCoords = IntVec2( tileX, endTileCoords.y );
 		map->SetTileTypeWithCoords( startYCoords, m_wallType );
 		map->SetTileRoom( startYCoords, true );
 		map->SetTileSolid( startYCoords, true );
 		tempRoom->AddRoomEdgeTileCoords( startYCoords );
 		
 		map->SetTileTypeWithCoords( endYCoords, m_wallType );
 		map->SetTileRoom( endYCoords, true );
 		map->SetTileSolid( endYCoords, true );
 		tempRoom->AddRoomEdgeTileCoords( endYCoords );
 	}
 
 	for( int tileY = startTileCoords.y; tileY <= endTileCoords.y; tileY++ ) {
 		IntVec2 startXCoords = IntVec2( startTileCoords.x, tileY );
 		IntVec2 endXCoords = IntVec2( endTileCoords.x, tileY );
 
 		map->SetTileTypeWithCoords( startXCoords, m_wallType );
 		map->SetTileRoom( startXCoords, true );
 		map->SetTileSolid( startXCoords, true );
 		tempRoom->AddRoomEdgeTileCoords( startXCoords );
 
 		map->SetTileTypeWithCoords( endXCoords, m_wallType );
 		map->SetTileRoom( endXCoords, true );
 		map->SetTileSolid( endXCoords, true );
 		tempRoom->AddRoomEdgeTileCoords( endXCoords );
 	}
	
	tempRoom->m_floorType = m_floorType;
	tempRoom->m_map = map;
	tempRoom->m_startCoords = startTileCoords;
	tempRoom->m_doorType = m_doorType;
	//map->AddRoom( tempRoom );
}
