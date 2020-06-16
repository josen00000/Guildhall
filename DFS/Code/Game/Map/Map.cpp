#include "Map.hpp"
#include <stack>
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/MapGenStep.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext*	g_theRenderer;
extern Physics2D*		g_thePhysics;

Map::Map( std::string name, MapDefinition* definition )
	:m_name(name)
	,m_definition(definition)
{
	static int seed = 0;
	m_rng = new RandomNumberGenerator( seed );
	seed++;
	CreateMapFromDefinition();
}

Map* Map::CreateMap( std::string name, MapDefinition* definition )
{
	Map* newMap = new Map( name, definition );
	return newMap;
}

void Map::RenderMap()
{
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->SetDiffuseTexture( tileTexture );
	g_theRenderer->DrawVertexVector( m_tilesVertices );
	
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawVertexVector( m_mazeVertices );
	//g_theRenderer->DrawAABB2D( AABB2( Vec2::ZERO, Vec2( 15 ) ), Rgba8::RED );
}

TileDirection Map::GetRevertTileDirection( TileDirection dirt )
{
	switch( dirt )
	{
	case DIRECTION_NONE:	return DIRECTION_NONE;
	case DIRECTION_UP:		return DIRECTION_DOWN;
	case DIRECTION_DOWN:	return DIRECTION_UP;
	case DIRECTION_LEFT:	return DIRECTION_RIGHT;
	case DIRECTION_RIGHT:	return DIRECTION_LEFT;
	}
	return DIRECTION_NONE;
}

Tile& Map::GetTileWithTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return m_tiles[tileIndex];
}

TileTypes Map::GetNeighborTypes( int tileIndex ) 
{
	IntVec2 tileCoords = GetTileCoordsWithTileIndex( tileIndex );
	TileTypes neighborTypes = GetNeighborTypes( tileCoords );
	return neighborTypes;
}

TileTypes Map::GetNeighborTypes( IntVec2 tileCoords ) 
{
	TileTypes neighborTypes;
	for( int tileX = -1; tileX < 1; tileX++ ) {
		for( int tileY = -1; tileY < 1; tileY++ ) {
			IntVec2 neighborTileCoords = tileCoords + IntVec2( tileX, tileY );
			if( !IsTileCoordsValid( neighborTileCoords ) ){ continue; }

			Tile& neighborTile = GetTileWithTileCoords( neighborTileCoords );
			TileType neighborType = neighborTile.GetTileType();
			bool isTypeExist = false;
			for( int i = 0; i < neighborTypes.size(); i++ ) {
				if( neighborTypes[i].compare( neighborType ) == 0 ){ 
					isTypeExist = true; 
					break;
				}
			}

			if( isTypeExist ) {
				continue;
			}
			neighborTypes.push_back( neighborType );
		}
	}
	return neighborTypes;
}

TileTypes Map::GetSolidNeighborTypes( IntVec2 tileCoords )
{
	TileTypes solidNeighborTypes = GetNeighborTypes( tileCoords );
	for( int i = 0; i < solidNeighborTypes.size(); i++ ) {
		if( TileDefinition::s_definitions[solidNeighborTypes[i]]->m_allowsWalk ) {
			solidNeighborTypes.erase( solidNeighborTypes.begin() + i );
		}
	}
	return solidNeighborTypes;
}

TileTypes Map::GetSolidNeighborTypes( int tileIndex )
{
	TileTypes solidNeighborTypes = GetNeighborTypes( tileIndex );
	for( int i = 0; i < solidNeighborTypes.size(); i++ ) {
		if( TileDefinition::s_definitions[solidNeighborTypes[i]]->m_allowsWalk ) {
			solidNeighborTypes.erase( solidNeighborTypes.begin() + i );
		}
	}
	return solidNeighborTypes;
}

bool Map::IsTileRoom( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileindex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileRoom( tileindex );
}

bool Map::IsTileRoom( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_ROOM );	
}

bool Map::IsTileExit( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileExit( tileIndex );
}

bool Map::IsTileExit( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_EXIT );	
}

bool Map::IsTileStart( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileStart( tileIndex );
}

bool Map::IsTileStart( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_START );	
}

bool Map::IsTileEdge( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileEdge( tileIndex );
}

bool Map::IsTileEdge( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_EDGE );
}

bool Map::IsTileSolid( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileSolid( tileIndex );
}

bool Map::IsTileSolid( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_SOLID );
}

bool Map::IsTileReachable( IntVec2 tileCoords ) const
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileReachable( tileIndex );
}

bool Map::IsTileReachable( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_REACHABLE );
}

bool Map::IsTileVisited( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileVisited( tileIndex );
}

bool Map::IsTileVisited( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_VISITED );
}

void Map::SetName( std::string name ) 
{
	m_name = name;
}

void Map::SetWidth( int width )
{
	m_width = width;
}

void Map::SetHeight( int height )
{
	m_height = height;
}

void Map::SetRoomNum( int roomNum )
{
	m_roomNum = roomNum;
}

void Map::AddRoomNum( int roomNum )
{
	m_roomNum += roomNum;
}

void Map::SetTileType( IntVec2 tileCoords, TileType type )
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	SetTileType( tileIndex, type );
	//Vec2 tileWorldPos = tempTile.GetWorldPos();
	//AABB2 tileBound = tempTile.GetTileBounds();
// 	Rigidbody2D* tempRb2D = g_thePhysics->CreateRigidbody( tileWorldPos );
// 	PolygonCollider2D* tempCol = g_thePhysics->CreatePolyCollider( tileBound, tempRb2D );
	//tempRb2D->SetCollider( (Collider2D*)tempCol );
}

void Map::SetTileType( int tileX, int tileY, TileType type )
{
	SetTileType( IntVec2( tileX, tileY ), type );
}

void Map::SetTileType( int tileIndex, TileType type )
{
	Tile& tempTile = m_tiles[tileIndex];
	tempTile.SetTileType( type );
	TileDefinition* tempTileDef = tempTile.GetTileDef();
	SetTileSolid( tileIndex, !tempTileDef->m_allowsWalk );
}

void Map::SetTileConnectTo( IntVec2 tileCoords, TileDirection dirt )
{
	Tile& tempTile = GetTileWithTileCoords( tileCoords );
	tempTile.SetConnectTo( (ConnectState)dirt, true );
}

void Map::AddRoom( Room room )
{
	m_rooms.push_back( room );
}

void Map::SetTileRoom( IntVec2 tileCoords, bool isRoom )
{
	SetTileAttrs( tileCoords, TILE_IS_ROOM, isRoom );
}

void Map::SetTileRoom( int tileIndex, bool isRoom )
{
	SetTileAttrs( tileIndex, TILE_IS_ROOM, isRoom );
}

void Map::SetTileEdge( IntVec2 tileCoords, bool isEdge )
{
	SetTileAttrs( tileCoords, TILE_IS_EDGE, isEdge );
}

void Map::SetTileEdge( int tileIndex, bool isEdge )
{
	SetTileAttrs( tileIndex, TILE_IS_EDGE, isEdge );
}

void Map::SetTileStart( IntVec2 tileCoords, bool isStart )
{
	SetTileAttrs( tileCoords, TILE_IS_START, isStart );
}

void Map::SetTileStart( int tileIndex, bool isStart )
{
	SetTileAttrs( tileIndex, TILE_IS_START, isStart );
}

void Map::SetTileExit( IntVec2 tileCoords, bool isExit )
{
	SetTileAttrs( tileCoords, TILE_IS_EXIT, isExit );
}

void Map::SetTileExit( int tileIndex, bool isExit )
{
	SetTileAttrs( tileIndex, TILE_IS_EXIT, isExit );
}

void Map::SetTileSolid( IntVec2 tileCoords, bool isSolid )
{
	SetTileAttrs( tileCoords, TILE_IS_SOLID, isSolid );
}

void Map::SetTileSolid( int tileIndex, bool isSolid )
{
	SetTileAttrs( tileIndex, TILE_IS_SOLID, isSolid );
}

void Map::SetTileReachable( IntVec2 tileCoords, bool isReachable )
{
	SetTileAttrs( tileCoords, TILE_IS_REACHABLE, isReachable );
}

void Map::SetTileReachable( int tileIndex, bool isReachable )
{
	SetTileAttrs( tileIndex, TILE_IS_REACHABLE, isReachable );
}

void Map::SetTileVisited( IntVec2 tileCoords, bool isVisited )
{
	SetTileAttrs( tileCoords, TILE_IS_VISITED, isVisited );
}

void Map::SetTileVisited( int tileIndex, bool isVisited )
{
	SetTileAttrs( tileIndex, TILE_IS_VISITED, isVisited );
}

IntVec2 Map::GetTileCoordsWithTileIndex( int tileIndex ) const
{
	IntVec2 tileCoords;
	tileCoords.x = tileIndex % m_width;
	tileCoords.y = tileIndex / m_width;
	return tileCoords;
}

IntVec2 Map::GetRandomInsideTileCoords() const
{
	IntVec2 tileCoords;
	do 
	{
		tileCoords.x = m_rng->RollRandomIntInRange( 0, m_width );
		tileCoords.y = m_rng->RollRandomIntInRange( 0, m_height );
	} while ( !IsTileCoordsInside( tileCoords ));
	return tileCoords;
}

IntVec2 Map::GetRandomInsideWalkableTileCoords() const
{
	bool isTileValid = false;
	IntVec2 tileCoords;
	while( !isTileValid ) {
		tileCoords = GetRandomInsideTileCoords();
		isTileValid = true;

		if( IsTileRoom( tileCoords ) ) {
			isTileValid = false;
		}
		if( IsTileSolid( tileCoords ) ) {
			isTileValid = false;
		}

	}
	return tileCoords;
}

int Map::GetTileIndexWithTileCoords( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_AND_DIE( Stringf( "tile coords x = %i, y = %i not valid", tileCoords.x, tileCoords.y ) );
	}
	int tileIndex = tileCoords.x + ( tileCoords.y * m_width );
	return tileIndex;
}

bool Map::IsTileCoordsValid( IntVec2 tileCoords ) const
{
	return !(( tileCoords.x >= m_width || tileCoords.x < 0 ) || ( tileCoords.y < 0 || tileCoords.y >= m_height ));
}

bool Map::IsTileOfTypeWithCoords( TileType type, IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ 
		//ERROR_AND_DIE( "invalid tile coords in tile type check!" );
		return false;
	}
	//if( !IsTileInside( tileCoords ) ){ return false; }

	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	const Tile& checkTile = m_tiles[tileIndex];
	return checkTile.IsType( type );
}

bool Map::IsTileOfTypeWithIndex( TileType type, int tileIndex ) const
{
	const Tile& checkTile = m_tiles[tileIndex];
	return checkTile.IsType( type );
}

bool Map::IsTileOfTypeInsideWithCoords( TileType type, IntVec2 tileCoords ) const
{
	if( !IsTileCoordsInside( tileCoords ) ){ return false; }

	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	const Tile& checkTile = m_tiles[tileIndex];
	return checkTile.IsType( type );
}

bool Map::IsTileCoordsInside( const IntVec2& tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	if( IsTileEdge( tileCoords ) ){ return false; }

	return true;
}


void Map::CreateMapFromDefinition()
{
	m_width = m_definition->GetWidth();
	m_height = m_definition->GetHeight();

	bool isValidMap = false;
	while( !isValidMap ){
		PopulateTiles();
		isValidMap = true;
	}
}

void Map::PopulateTiles()
{
	InitializeTiles();
	GenerateEdgeTiles( 1 );
	RunMapGenSteps();
	RandomGenerateStartAndExitCoords();
	GenerateMazeTiles();
	GenerateDoorTiles();
	CheckTilesReachability();
	PushTileVertices();
}

void Map::InitializeTiles()
{
	int tileNum = m_height * m_width;
	m_tiles.reserve( tileNum );
	m_tileAttributes.resize( tileNum );

	for( int i = 0; i < tileNum; i++ ) {
		IntVec2 tileCoords = GetTileCoordsWithTileIndex( i );
		m_tiles.emplace_back(  tileCoords, m_definition->GetFillType() );
		SetTileReachable( i, true );
	}
}

void Map::GenerateEdgeTiles( int edgeThick )
{
	TileType edgeType = m_definition->GetEdgeType();
	for( int tileX = 0; tileX < m_width; tileX++ ) {
		for( int tileY = 0; tileY < edgeThick; tileY++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( tileX, ( m_height - 1 - tileY ), edgeType );
			SetTileEdge( IntVec2( tileX, tileY ), true );
			SetTileEdge( IntVec2( tileX, ( m_height - 1 - tileY )), true );
			SetTileSolid( IntVec2( tileX, tileY ), true );
			SetTileSolid( IntVec2( tileX, ( m_height - 1 - tileY )), true );
		}
	}

	for( int tileY = 0; tileY < m_height; tileY++ ) {
		for( int tileX = 0; tileX < edgeThick; tileX++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( ( m_width - 1 - tileX ), tileY, edgeType );
			SetTileEdge( IntVec2( tileX, tileY ), true );
			SetTileEdge( IntVec2(( m_width - 1 - tileX ), tileY ), true );
			SetTileSolid( IntVec2( tileX, tileY ), true );
			SetTileSolid( IntVec2((m_width - 1 - tileX ), tileY ), true );
		}
	}
}

void Map::RunMapGenSteps()
{
	const std::vector<MapGenStep*>& mapGenSteps = m_definition->GetMapGenSteps();

	for( int i = 0; i < mapGenSteps.size(); i++ ) {
		mapGenSteps[i]->RunStep( this );
	}
}

void Map::GenerateMazeTiles()
{
	std::stack<IntVec2> mazeStack;
	SetTileVisited( m_startCoords, true );
	mazeStack.push( m_startCoords );

	while( !mazeStack.empty() ) {
		IntVec2 tempTileCoords = mazeStack.top();

		if( IsTileCoordsDeadEnd( tempTileCoords ) ){
			SetTileVisited( tempTileCoords, true );
			mazeStack.pop();
			continue;
		}

		TileDirection validNeighborTileDirt = GetValidNeighborDirection( tempTileCoords );
		SetTileConnectTo( tempTileCoords, validNeighborTileDirt );
		IntVec2 neighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, validNeighborTileDirt );
		TileDirection revertNeighborTileDirt = GetRevertTileDirection( validNeighborTileDirt );
		SetTileConnectTo( neighborCoords, revertNeighborTileDirt );

		SetTileVisited( neighborCoords, true );
		mazeStack.push( neighborCoords );
		if( IsTileOfTypeWithCoords( "Water", neighborCoords ) ) {
			bool ab = IsTileSolid( neighborCoords );
			int a = 0;
		}
	}

}

void Map::GenerateDoorTiles()
{
	for( int i = 0; i < m_roomNum; i++ ) {
		Room tempRoom = m_rooms[i];
		while( true ) {
			IntVec2 doorCoords = IntVec2();
			IntVec2 doorStartTileCoords = tempRoom.m_startCoords;
			IntVec2 doorEndTileCoords = tempRoom.m_startCoords + IntVec2( tempRoom.m_width, tempRoom.m_height );

			int randAxis = m_rng->RollRandomIntInRange( 0, 1 );
			if( randAxis == 0 ) {
				doorCoords.x = m_rng->RollRandomIntInRange( doorStartTileCoords.x + 1, doorEndTileCoords.x - 1 );
				int randY = m_rng->RollRandomIntInRange( 0, 1 );
				if( randY == 0 ) {
					doorCoords.y = doorStartTileCoords.y;
				}
				else {
					doorCoords.y = doorEndTileCoords.y;
				}
			}
			else {
				doorCoords.y = m_rng->RollRandomIntInRange( doorStartTileCoords.y + 1, doorEndTileCoords.y - 1 );
				int randX = m_rng->RollRandomIntInRange( 0, 1 );
				if( randX == 0 ) {
					doorCoords.x = doorStartTileCoords.x;
				}
				else{
					doorCoords.x = doorEndTileCoords.x;
				}
			}
			
			// check if door coords valid
			IntVec2 upCoords = doorCoords + IntVec2( 0, 1 );
			IntVec2 downCoords = doorCoords + IntVec2( 0, -1 );
			IntVec2 leftCoords = doorCoords + IntVec2( -1, 0 );
			IntVec2 rightCoords = doorCoords + IntVec2( 1, 0 );
			if( IsTileVisited( upCoords ) || IsTileVisited( downCoords ) || IsTileVisited( leftCoords ) || IsTileVisited( rightCoords ) ) {
				SetTileType( doorCoords, tempRoom.m_doorType );
				break;
			}
		}
	}
}

bool Map::IsTileCoordsDeadEnd( IntVec2 tileCoords )
{
	bool isDeadEnd = true;
	IntVec2 upNeighborCoords	= tileCoords + IntVec2( 0, 1 );
	IntVec2 downNeighborCoords	= tileCoords + IntVec2( 0, -1 );
	IntVec2 leftNeighborCoords	= tileCoords + IntVec2( -1, 0 );
	IntVec2 rightNeighborCoords	= tileCoords + IntVec2( 1, 0 );

	if( !IsTileVisited( upNeighborCoords ) && !IsTileSolid( upNeighborCoords ))			{ isDeadEnd = false; }
	if( !IsTileVisited( downNeighborCoords ) && !IsTileSolid( downNeighborCoords ))		{ isDeadEnd = false; }
	if( !IsTileVisited( leftNeighborCoords ) && !IsTileSolid( leftNeighborCoords ))		{ isDeadEnd = false; }
	if( !IsTileVisited( rightNeighborCoords ) && !IsTileSolid( rightNeighborCoords ))	{ isDeadEnd = false; }

	return isDeadEnd;
}

IntVec2 Map::GetNeighborTileCoordsInDirection( IntVec2 tileCoords, TileDirection dirt )
{
	switch( dirt )
	{
	case DIRECTION_NONE:
		ERROR_AND_DIE( "wrong direction in find neighbor!" );
		break;
	case DIRECTION_UP:		return ( tileCoords + IntVec2( 0, 1 ));
	case DIRECTION_DOWN:	return ( tileCoords + IntVec2( 0, -1 ));
	case DIRECTION_LEFT:	return ( tileCoords + IntVec2( -1, 0 ));
	case DIRECTION_RIGHT:	return ( tileCoords + IntVec2( 1, 0 ));
	}

	return IntVec2::ZERO;
}

TileDirection Map::GetValidNeighborDirection( IntVec2 tileCoords )
{
	bool isNeighborValid = false;
	TileDirection validNeighborDirection = DIRECTION_NONE;
	IntVec2 neighborCoords;
	IntVec2 upNeighborCoords	= tileCoords + IntVec2( 0, 1 );
	IntVec2 downNeighborCoords	= tileCoords + IntVec2( 0, -1 );
	IntVec2 leftNeighborCoords	= tileCoords + IntVec2( -1, 0 );
	IntVec2 rightNeighborCoords	= tileCoords + IntVec2( 1, 0 );

	while( !isNeighborValid ) {
		validNeighborDirection = (TileDirection)m_rng->RollRandomIntInRange( 1, 4 );
		if( validNeighborDirection == DIRECTION_UP ) { neighborCoords = upNeighborCoords; }
		if( validNeighborDirection == DIRECTION_DOWN ) { neighborCoords = downNeighborCoords; }
		if( validNeighborDirection == DIRECTION_LEFT ) { neighborCoords = leftNeighborCoords; }
		if( validNeighborDirection == DIRECTION_RIGHT ) { neighborCoords = rightNeighborCoords; }
		if( validNeighborDirection == DIRECTION_NONE ) {
			ERROR_RECOVERABLE( "direction is none" );
		}
		if( !IsTileVisited( neighborCoords ) && !IsTileSolid( neighborCoords ) ) { isNeighborValid = true; }
	}
	
	return validNeighborDirection;
}

void Map::PushTileVertices()
{
	m_tilesVertices.clear();
	m_mazeVertices.clear();
	m_tilesVertices.reserve( m_tiles.size() * 6 );
	for( int i = 0; i < m_tiles.size(); i++ ) {
		Tile tempTile = m_tiles[i];
		AABB2 tempBound = tempTile.GetTileBounds();
		TileDefinition* tempTileDef = tempTile.GetTileDef();
		AppendVertsForAABB2D( m_tilesVertices, tempBound, tempTileDef->GetTintColor(), tempTileDef->GetSpriteUVAtMins(), tempTileDef->GetSpriteUVAtMaxs() );
		
		// debug draw for connection
		Vec2 tempTilePos = tempTile.GetWorldPos();
		if( tempTile.IsConnectTo( CONNECT_TO_LEFT ) ) {
			LineSegment2 debugLine = LineSegment2( tempTilePos, tempTilePos + Vec2( -1, 0 ) );
			AABB2 lineBox = AABB2( ( debugLine.GetStartPos() - Vec2( 0.05f ) ), ( debugLine.GetEndPos() + Vec2( 0.05f ) ) );
			AppendVertsForAABB2D( m_mazeVertices, lineBox, Rgba8::RED, Vec2::ZERO, Vec2::ZERO );
		}

		if( tempTile.IsConnectTo( CONNECT_TO_RIGHT ) ) {
			LineSegment2 debugLine = LineSegment2( tempTilePos, tempTilePos + Vec2( 1, 0 ) );
			AABB2 lineBox = AABB2( (debugLine.GetStartPos() - Vec2( 0.05f )), (debugLine.GetEndPos() + Vec2( 0.05f )) );
			AppendVertsForAABB2D( m_mazeVertices, lineBox, Rgba8::RED, Vec2::ZERO, Vec2::ZERO );
		}

		if( tempTile.IsConnectTo( CONNECT_TO_UP ) ) {
			LineSegment2 debugLine = LineSegment2( tempTilePos, tempTilePos + Vec2( 0, 1 ) );
			AABB2 lineBox = AABB2( (debugLine.GetStartPos() - Vec2( 0.05f )), (debugLine.GetEndPos() + Vec2( 0.05f )) );
			AppendVertsForAABB2D( m_mazeVertices, lineBox, Rgba8::RED, Vec2::ZERO, Vec2::ZERO );
		}

		if( tempTile.IsConnectTo( CONNECT_TO_DOWN ) ) {
			LineSegment2 debugLine = LineSegment2( tempTilePos, tempTilePos + Vec2( 0, -1 ) );
			AABB2 lineBox = AABB2( (debugLine.GetStartPos() - Vec2( 0.05f )), (debugLine.GetEndPos() + Vec2( 0.05f )) );
			AppendVertsForAABB2D( m_mazeVertices, lineBox, Rgba8::RED, Vec2::ZERO, Vec2::ZERO );
		}

	}
}

void Map::RandomGenerateStartAndExitCoords()
{
	IntVec2 startCoords = GetRandomInsideWalkableTileCoords();
	SetTileStart( startCoords, true );
	m_startCoords = startCoords;

	IntVec2 exitCoords = GetRandomInsideWalkableTileCoords();
	while( IsTileStart( exitCoords ) ) {
		exitCoords = GetRandomInsideWalkableTileCoords();
	}
	SetTileExit( exitCoords, true );
	m_exitCoords = exitCoords;
}

void Map::CheckTilesReachability()
{
	bool notExistUnreachableTile = false;
	while( !notExistUnreachableTile ) {
		notExistUnreachableTile = true;
		for( int i = 0; i < m_tiles.size(); i++ ) {
			if( !IsTileSolid( i ) && !IsTileVisited( i ) && !IsTileRoom( i ) ) {
				TileTypes solidNeighborTypes = GetSolidNeighborTypes( i );
				int randomSelect = m_rng->RollRandomIntInRange( 1, (int)solidNeighborTypes.size()) - 1;
				notExistUnreachableTile = false;
				SetTileType( i, solidNeighborTypes[randomSelect] );
			}
		}
	}
}

void Map::SetTileAttrs( IntVec2 tileCoords, TileAttribute attr, bool state )
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	SetTileAttrs( tileIndex, attr, state );
}

void Map::SetTileAttrs( int tileIndex, TileAttribute attr, bool state )
{
	TileAttributeBitFlag attrBit = GetTileAttrBitFlagWithAttr( attr );
	uint& tileAttrsState = m_tileAttributes[tileIndex];
	if( state ) {
		tileAttrsState = tileAttrsState | attrBit;
	}
	else {
		tileAttrsState = tileAttrsState & ~attrBit;
	}
}

bool Map::isTileAttrs( IntVec2 tileCoords, TileAttribute attr ) const
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return isTileAttrs( tileIndex, attr );
}

bool Map::isTileAttrs( int tileIndex, TileAttribute attr ) const
{
	uint tempTileAttrsState = m_tileAttributes[tileIndex];
	TileAttributeBitFlag attrFlag = GetTileAttrBitFlagWithAttr( attr );
	return ( ( tempTileAttrsState & attrFlag ) == attrFlag );
}

TileAttributeBitFlag Map::GetTileAttrBitFlagWithAttr( TileAttribute attr ) const
{
	switch( attr )
	{
	case TILE_IS_EDGE:		return TILE_IS_EDGE_BIT;
	case TILE_IS_START:		return TILE_IS_START_BIT;
	case TILE_IS_EXIT:		return TILE_IS_EXIT_BIT;
	case TILE_IS_ROOM:		return TILE_IS_ROOM_BIT;
	case TILE_IS_SOLID:		return TILE_IS_SOLID_BIT;
	case TILE_IS_VISITED:	return TILE_IS_VISITED_BIT;
	case TILE_IS_REACHABLE:	return TILE_IS_REACHABLE_BIT;
	}
	ERROR_RECOVERABLE( "no attribute!" );
	return TILE_NON_ATTR_BIT;
}
