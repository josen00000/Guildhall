#include "Map.hpp"
#include <stack>
#include <time.h>
#include "Game/Player.hpp"
#include "Game/Item.hpp"
#include "Game/Game.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/MapGenStep.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Camera.hpp"

extern RenderContext*	g_theRenderer;
extern Physics2D*		g_thePhysics;
extern AudioSystem*		g_theAudioSystem;
extern Game*			g_theGame;
extern Camera* g_debugCamera;
extern Camera* g_gameCamera;

Map::Map( std::string name, MapDefinition* definition )
	:m_name(name)
	,m_definition(definition)
{
	static int seed = 0;
	m_rng = new RandomNumberGenerator( seed );
	seed++;
	CreateMapFromDefinition();
	CreateActors();
	m_fightTimer = new Timer();
}

Map* Map::CreateMap( std::string name, MapDefinition* definition )
{
	Map* newMap = new Map( name, definition );
	return newMap;
}

void Map::UpdateMap( float deltaSeconds )
{
	CheckCollision();
	CheckFight();
	if( m_fightEnemy && m_isFighting ) {
		UpdateFight( m_fightEnemy, deltaSeconds );
	}
	m_player->UpdatePlayer( deltaSeconds );	
	CheckIfCollectKey(); 
	for( int i = 0; i < m_enemies.size(); i++ ) {
		m_enemies[i]->UpdateActor( deltaSeconds );
		m_enemies[i]->UpdateActorAnimation( deltaSeconds );
	}
	
	if( m_enemies.size() == 0 ) {
		g_theGame->SetGameEnd();
	}
	CheckPlayerOnLava();
}

void Map::RenderMap()
 {
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->SetDiffuseTexture( tileTexture );
	g_theRenderer->DrawVertexVector( m_tilesVertices );
	
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawVertexVector( m_mazeVertices );

 	m_player->RenderPlayer();
	for( int i = 0; i < m_enemies.size(); i++ ) {
		m_enemies[i]->RenderActor();
	}

	if( !m_player->GetHasKey() ) {
		Texture* playerTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/key.png" );
		g_theRenderer->SetDiffuseTexture( playerTexture );
		m_key->RenderItem();
	}

	if( m_isFighting ) {
		RenderFightAnimation();
	}
}

Vec2 Map::GetWorldPosWithTileCoords( IntVec2 tileCoords )
{
	return Vec2( tileCoords );
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

void Map::GetNeighborsCoords( IntVec2 tileCoords, IntVec2* neighborCoords )
{
	neighborCoords[0] = tileCoords + IntVec2( 0, 1 );
	neighborCoords[1] = tileCoords + IntVec2( 0, -1 );
	neighborCoords[2] = tileCoords + IntVec2( -1, 0 );
	neighborCoords[3] = tileCoords + IntVec2( 1, 0 );
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

bool Map::IsTileMapEdge( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	return ( tileCoords.x == 0 || tileCoords.x == m_width - 1 || tileCoords.y == 0 || tileCoords.y == m_height - 1 );
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

bool Map::IsTileDoor( IntVec2 tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return IsTileDoor( tileIndex );
}

bool Map::IsTileDoor( int tileIndex ) const
{
	return isTileAttrs( tileIndex, TILE_IS_DOOR );
}

bool Map::IsTileReachable( int tileIndex ) const
{
	IntVec2 tileCoords = GetTileCoordsWithTileIndex( tileIndex );
	std::vector<IntVec2> neighborsTileCoords;
	neighborsTileCoords.push_back( tileCoords + IntVec2( 0, 1 ) );
	neighborsTileCoords.push_back( tileCoords + IntVec2( 0, -1 ) );
	neighborsTileCoords.push_back( tileCoords + IntVec2( 1, 0 ) );
	neighborsTileCoords.push_back( tileCoords + IntVec2( -1, 0 ) );

	int NumOfsolidNeighbors = 0;
	for( int i = 0; i < neighborsTileCoords.size(); i++ ) {
		if( IsTileSolid( neighborsTileCoords[i] ) ) {
			NumOfsolidNeighbors++;
		}
	}

	if( NumOfsolidNeighbors == 4 ){ return false; }
	return true;
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

void Map::SetTileNeighborsSolidWithDirt( IntVec2 tileCoords, TileDirection dirt )
{
	TileType edgeType = m_definition->GetEdgeType();
	if( dirt == DIRECTION_UP || dirt == DIRECTION_DOWN ) {
		IntVec2 neighborACoords	= tileCoords + IntVec2( 1, 0 );
		IntVec2 neighborBCoords	= tileCoords + IntVec2( -1, 0 );
		SetTileType( neighborACoords, edgeType );
		SetTileType( neighborBCoords, edgeType );
	}
	if( dirt == DIRECTION_LEFT || dirt == DIRECTION_RIGHT ) {
		IntVec2 neighborACoords	= tileCoords + IntVec2( 0, 1 );
		IntVec2 neighborBCoords	= tileCoords + IntVec2( 0, -1 );
		SetTileType( neighborACoords, edgeType );
		SetTileType( neighborBCoords, edgeType );
	}
}

void Map::AddRoom( Room* room )
{
	m_rooms.push_back( room );
}

void Map::AddMaze( Maze* maze )
{
	m_mazes.push_back( maze );
}

void Map::SetTileRoom( IntVec2 tileCoords, bool isRoom )
{
	SetTileAttrs( tileCoords, TILE_IS_ROOM, isRoom );
}

void Map::SetTileRoom( int tileIndex, bool isRoom )
{
	SetTileAttrs( tileIndex, TILE_IS_ROOM, isRoom );
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

void Map::SetTileVisited( IntVec2 tileCoords, bool isVisited )
{
	SetTileAttrs( tileCoords, TILE_IS_VISITED, isVisited );
}

void Map::SetTileVisited( int tileIndex, bool isVisited )
{
	SetTileAttrs( tileIndex, TILE_IS_VISITED, isVisited );
}

void Map::SetTileDoor( IntVec2 tileCoords, bool isDoor )
{
	SetTileAttrs( tileCoords, TILE_IS_DOOR, isDoor );
}

void Map::SetTileDoor( int tileIndex, bool isDoor )
{
	SetTileAttrs( tileIndex, TILE_IS_DOOR, isDoor );
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
	if( IsTileMapEdge( tileCoords ) ){ return false; }

	return true;
}


void Map::CreateMapFromDefinition()
{
	m_width = m_definition->GetWidth();
	m_height = m_definition->GetHeight();
	m_mazeFloorType = m_definition->GetFillType();
	m_mazeWallType = m_definition->GetEdgeType();
	Texture* fightTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Explosion_5x5.png" );
	m_fightSpriteSheet = new SpriteSheet( fightTexture, IntVec2( 5, 5 ) );
	int fightSpriteIndexes[6]{ 0, 1, 2, 3, 23, 24 };
	m_fightAnimDef = new SpriteAnimDefinition( *m_fightSpriteSheet, fightSpriteIndexes, 6, 0.001f );


	clock_t start = clock();

	bool isValidMap = false;
	while( !isValidMap ){
		PopulateTiles();
		isValidMap = true;
	}
	clock_t end = clock();
	double time_taken = double( (end - start)/CLOCKS_PER_SEC );
}

void Map::PopulateTiles()
{
	InitializeTiles();
	GenerateEdgeTiles( 1 );
 	RunMapGenSteps();
  	RandomGenerateStartAndExitCoords();
 	EliminateSingleLavaAndWater();
  	GenerateMazeTiles();
   	GenerateDoorTiles();
   	EliminateDeadEnds();
	CheckTilesReachability();
   	CreateKey();
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
	}
}

void Map::GenerateEdgeTiles( int edgeThick )
{
	TileType edgeType = m_definition->GetEdgeType();
	for( int tileX = 0; tileX < m_width; tileX++ ) {
		for( int tileY = 0; tileY < edgeThick; tileY++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( tileX, ( m_height - 1 - tileY ), edgeType );
			SetTileSolid( IntVec2( tileX, tileY ), true );
			SetTileSolid( IntVec2( tileX, ( m_height - 1 - tileY )), true );
		}
	}

	for( int tileY = 0; tileY < m_height; tileY++ ) {
		for( int tileX = 0; tileX < edgeThick; tileX++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( ( m_width - 1 - tileX ), tileY, edgeType );
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
	Maze* tempMaze = nullptr;
	bool isAbleMaze = true;
	int mazeNum = 0;
	while( isAbleMaze ) {
		isAbleMaze = false;
		for( int i = 0; i < m_tiles.size(); i++ ) {
			if( !IsTileVisited( i ) && !IsTileSolid( i ) && !IsTileRoom( i ) ) {
				IntVec2 startCoords = GetTileCoordsWithTileIndex( i );
				SetTileVisited( startCoords, true );
				mazeStack.push( startCoords );
				isAbleMaze = true;
				tempMaze = new Maze( this );
				tempMaze->SetLayer( m_mazeAndRoomCurrentLayer );
				m_mazeAndRoomCurrentLayer++;
				break;
			}
		}
		mazeNum++;

		while( !mazeStack.empty() ) {
			IntVec2 tempTileCoords = mazeStack.top();

			if( IsTileCoordsDeadEnd( tempTileCoords ) ){
				mazeStack.pop();
				if( IsTileCoordsWalkableDeadEnd( tempTileCoords ) ) {
					tempMaze->m_deadEndtileCoords.push( tempTileCoords );
				}
				continue;
			}

 			TileDirection validNeighborTileDirt = GetValidNeighborDirection( tempTileCoords );
 			IntVec2 neighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, validNeighborTileDirt );
			//TileDirection revertNeighborTileDirt = GetRevertTileDirection( validNeighborTileDirt );
			
			if( m_rng->RollPercentChance( m_mazeEdgePercentage ) ) {
				// new version
				if( validNeighborTileDirt == DIRECTION_UP || validNeighborTileDirt == DIRECTION_DOWN ) {
					IntVec2 edgeLeftNeighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, DIRECTION_LEFT );
					IntVec2 edgeRightNeighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, DIRECTION_RIGHT );
					
					SetMazeNeighborEdge( edgeLeftNeighborCoords );
					SetMazeNeighborEdge( edgeRightNeighborCoords );
					tempMaze->AddMazeTileCoords( edgeLeftNeighborCoords );
					tempMaze->AddMazeTileCoords( edgeRightNeighborCoords );
				}
				if( validNeighborTileDirt == DIRECTION_LEFT || validNeighborTileDirt == DIRECTION_RIGHT ) {
					IntVec2 edgeUpNeighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, DIRECTION_UP );
					IntVec2 edgeDownNeighborCoords = GetNeighborTileCoordsInDirection( tempTileCoords, DIRECTION_DOWN );
					
					SetMazeNeighborEdge( edgeUpNeighborCoords );
					SetMazeNeighborEdge( edgeDownNeighborCoords );
					tempMaze->AddMazeTileCoords( edgeUpNeighborCoords );
					tempMaze->AddMazeTileCoords( edgeDownNeighborCoords );
				}
			}
			SetTileVisited( neighborCoords, true );
			tempMaze->AddMazeTileCoords( neighborCoords );
			mazeStack.push( neighborCoords );
		}
		if( isAbleMaze ) {
			m_mazes.push_back( tempMaze );
			tempMaze = nullptr;
		}
	}
}

void Map::GenerateDoorTiles()
{
	std::stack<Room*> leftRooms;
	for( int i = 0; i < m_rooms.size(); i++ ) {
		leftRooms.push( m_rooms[i] );
	}

	while ( !leftRooms.empty() ){
		Room* tempRoom = leftRooms.top();
		tempRoom->SetLayer( 0 );
		int doorNum = 0;
		std::vector<IntVec2> edgeTileCoords = tempRoom->GetEdgeTileCoords();
		for( int j = 0; j < m_mazes.size(); j++ ) {
			Maze* tempMaze = m_mazes[j];

			bool isMazeConnectToRoom = false;

			for( int k = 0; k < edgeTileCoords.size(); k++ ) {
				if( IsEdgeConnectToMazeAndFloor( edgeTileCoords[k], tempRoom, tempMaze ) ) {
					isMazeConnectToRoom = true;
					break;
				}
			}
			if( isMazeConnectToRoom ) {
				if( doorNum != 0 && tempMaze->Getlayer() == 0 ) {
					if( m_rng->RollPercentChance( 0.5f ) ) {
						break;
					}
				}
				bool isDoorValid = false;
				while( !isDoorValid )
				{
					IntVec2 randomDoorCoords = tempRoom->GetRandomEdgeTileCoord();
					if( IsEdgeConnectToMazeAndFloor( randomDoorCoords, tempRoom, tempMaze ) ) {
						isDoorValid = true;
						SetTileType( randomDoorCoords, tempRoom->m_doorType );
						SetTileDoor( randomDoorCoords, true );
						doorNum++;
						tempMaze->SetLayer( 0 );
					}
				}
			}
		}
		leftRooms.pop();
	}
}

void Map::EliminateDeadEnds()
{
	int skipDeadEndChance = 0.3f;
	for( int i = 0; i < m_mazes.size(); i++ ) {
		Maze* tempMaze = m_mazes[i];
		std::stack<IntVec2> deadEnds = tempMaze->m_deadEndtileCoords;
		while( !deadEnds.empty() ) {
			IntVec2 deadEndTileCoods = deadEnds.top();
			deadEnds.pop();
			if( !IsTileCoordsWalkableDeadEnd( deadEndTileCoods ) ){ continue; }
			if( IsTileStart( deadEndTileCoods ) ){ continue; }
			SetTileType( deadEndTileCoods, m_mazeWallType );
			SetTileSolid( deadEndTileCoods, true );
			IntVec2 neighborCoords[4];
			GetNeighborsCoords( deadEndTileCoods, neighborCoords );
			if( m_rng->RollPercentChance( skipDeadEndChance ) ) {
				for( int j = 0; j < 4; j++ ) {
					if( IsTileCoordsWalkableDeadEnd( neighborCoords[j] ) ) {
						deadEnds.push( neighborCoords[j] );
					}
				}
			}
		}
	}
}

void Map::SetMazeNeighborEdge( IntVec2 tileCoords )
{
	if( !IsTileVisited( tileCoords ) && !IsTileExit( tileCoords ) && !IsTileStart( tileCoords ) && !IsTileSolid( tileCoords ) ) {
		SetTileType( tileCoords, m_mazeWallType );
		//SetTileVisited( tileCoords, true );
	}
}

bool Map::IsTileCoordsDeadEnd( IntVec2 tileCoords )
{
	bool isDeadEnd = true;
	if( IsTileSolid( tileCoords ) ){ return false; }
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

bool Map::IsTileCoordsWalkableDeadEnd( IntVec2 tileCoords )
{
	if( IsTileSolid( tileCoords ) ) { return false; }
	IntVec2 neighborCoords[4];
	GetNeighborsCoords( tileCoords, neighborCoords );

	int solidNeighborNum = 0;
	for( int i = 0; i < 4; i++ ) {
		if( IsTileSolid( neighborCoords[i] ) && !IsTileDoor( neighborCoords[i] ) ){ solidNeighborNum++; }
	}
	return ( solidNeighborNum == 3 );
}

bool Map::IsEdgeConnectToMazeAndFloor( IntVec2 const& tileCoords, Room const* room, Maze const* maze )
{
	IntVec2 neighborCoords[4];
	GetNeighborsCoords( tileCoords, neighborCoords );
	
	int mazeNeighborNum = 0;
	int floorNeighborNum = 0;
	for( int i = 0; i < 4; i++ ) {
		if( !IsTileCoordsInside( neighborCoords[i] ) ){ continue; }
		if( room->IsTileOfRoomFloor( neighborCoords[i] ) ){ floorNeighborNum++; }
		if( maze->isTileOfMaze( neighborCoords[i] ) && !IsTileSolid( neighborCoords[i] )){ mazeNeighborNum++; }
	}

	return ( mazeNeighborNum > 0 && floorNeighborNum > 0 );
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
	//Rgba8 debugColor = Rgba8::WHITE;
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
	TileType startType = m_definition->GetStartType();
	TileType endType = m_definition->GetEndType();
	IntVec2 startCoords = GetRandomInsideWalkableTileCoords();
	SetTileStart( startCoords, true );
	SetTileType( startCoords, startType );
	m_startCoords = startCoords;

// 	IntVec2 endCoords = GetRandomInsideWalkableTileCoords();
// 	while( IsTileStart( endCoords ) ) {
// 		endCoords = GetRandomInsideWalkableTileCoords();
// 	}
// 	SetTileExit( endCoords, true );
// 	SetTileType( endCoords, endType );
// 	m_endCoords = endCoords;
}

void Map::EliminateSingleLavaAndWater()
{
	for( int i = 0; i < m_tiles.size(); i++ ) {
		if( IsTileRoom( i ) ){ continue; }

		if( IsTileOfTypeWithIndex( "Lava", i ) ) {
			int numOfLavaNeighbors = GetNumOfNeighborTilesInTypeWithIndex( "Lava", i );
			if( numOfLavaNeighbors < 3 ) {
				SetTileType( i, m_mazeFloorType );
			}


		}
		if( IsTileOfTypeWithIndex( "Water", i ) ) {
			int numOfWaterNeighbors = GetNumOfNeighborTilesInTypeWithIndex( "Water", i );
			if( numOfWaterNeighbors < 3 ) {
				SetTileType( i, m_mazeFloorType );
				SetTileSolid( i, false );
			}
		}
	}
}

void Map::CheckTilesReachability()
{
	bool notExistUnreachableTile = false;
	while( !notExistUnreachableTile ) {
		notExistUnreachableTile = true;
		for( int i = 0; i < m_tiles.size(); i++ ) {
			int tileindex = GetTileIndexWithTileCoords( IntVec2( 10, 5 ) );
			if( i == tileindex ) {
				int a = 0;
			}
			if( !IsTileSolid( i ) && !IsTileReachable( i ) && !IsTileRoom( i ) ) {
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
	case TILE_IS_START:		return TILE_IS_START_BIT;
	case TILE_IS_EXIT:		return TILE_IS_EXIT_BIT;
	case TILE_IS_ROOM:		return TILE_IS_ROOM_BIT;
	case TILE_IS_SOLID:		return TILE_IS_SOLID_BIT;
	case TILE_IS_VISITED:	return TILE_IS_VISITED_BIT;
	case TILE_IS_DOOR:		return TILE_IS_DOOR_BIT;
	}
	ERROR_RECOVERABLE( "no attribute!" );
	return TILE_NON_ATTR_BIT;
}

int Map::GetNumOfNeighborTilesInTypeWithIndex( TileType type, int tileIndex ) const
{
	IntVec2 tileCoords = GetTileCoordsWithTileIndex( tileIndex );
	std::vector<IntVec2> neighborTileCoords;

	neighborTileCoords.push_back( tileCoords + IntVec2( 0, 1 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( 0, -1 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( -1, 0 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( 1, 0 ) );

	neighborTileCoords.push_back( tileCoords + IntVec2( 1, 1 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( -1, 1 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( 1, -1 ) );
	neighborTileCoords.push_back( tileCoords + IntVec2( -1, -1 ) );

	int numOfNeighborTiles = 0;
	for( int i = 0; i < neighborTileCoords.size(); i++ ) {
		if( IsTileOfTypeWithCoords( type, neighborTileCoords[i] ) ) {
			numOfNeighborTiles++;
		}
	}
	return numOfNeighborTiles;
}

void Map::CreateActors()
{
	CreatePlayer();
	CreateEnemies();
	EventArgs args;
	std::string resultString = "Kill all enemies to win!";
	args.SetValue( "message", resultString );
	g_theEventSystem->FireEvent( "DebugMessage", args );
}

void Map::CreatePlayer()
{
	m_player = Player::SpawnPlayerWithPos( ActorDefinition::s_definitions["Player"], Vec2( m_startCoords ) + Vec2( 0.5f ) );
}

void Map::CreateEnemies()
{
	for( int i = 0; i < m_roomNum; i++ ) {
		IntVec2 pos_int = m_rooms[i]->GetRandomFloorTileCoord();
		Vec2 pos = Vec2( pos_int ) + Vec2( 0.5f );
		Actor* tempActor = nullptr;
		if( m_rng->RollPercentChance( 0.05f ) ) {
			tempActor = Actor::SpawnActorWithPos( ActorDefinition::s_definitions["Ghost"], pos );
		}
		else {
			tempActor = Actor::SpawnActorWithPos( ActorDefinition::s_definitions["Clown"], pos );
		}
		m_enemies.push_back( tempActor );
	}
}

void Map::DeleteEnemy( Actor* enemy )
{
	for( int i = 0; i < m_enemies.size(); i++ ) {
		if( enemy == m_enemies[i] ) {
			m_enemies[i] = m_enemies.back();
			break;
		}
	}
	m_enemies.pop_back();
	delete enemy;
}

void Map::CreateKey()
{
	Maze* startMaze = nullptr;
	for( int i = 0; i < m_mazes.size(); i++ ) {
		Maze* tempMaze = m_mazes[i];
		if( tempMaze->isTileOfMaze( m_startCoords ) ) {
			startMaze = tempMaze;
			break;
		}
	}

	while( true ) {
		IntVec2 tileCoords = startMaze->GetRandomTileCoords();
		if( !IsTileSolid( tileCoords ) ) {
			m_key = new Item( Vec2( tileCoords ) + Vec2( 0.5f ) );
			break;
		}
	}
}

void Map::CheckIfCollectKey()
{
	if( m_player->GetHasKey() ){ return; }
	Vec2 playerPos = m_player->GetPosition();
	Vec2 keyPos = m_key->GetPosition();
	Vec2 disp = playerPos - keyPos;
	float length = disp.GetLength();
	if( length < m_player->GetPhysicRadius() ) {
		m_player->SetHasKey( true );
		EventArgs args;
		std::string resultString = "Find Key.";
		args.SetValue( "message", resultString );
		g_theEventSystem->FireEvent( "DebugMessage", args );
	}
}

void Map::CheckCollision()
{
	CheckTileCollision();
}

void Map::CheckTileCollision()
{
	CheckPlayerTileCollision();
}

void Map::CheckPlayerTileCollision()
{
	if( m_player == nullptr ){ return; }
	IntVec2 playerTileCoords = IntVec2( m_player->GetPosition() );

	IntVec2 downTileCoords	= playerTileCoords + IntVec2( 0, -1 );
	IntVec2 upTileCoords	= playerTileCoords + IntVec2( 0, 1 );
	IntVec2 leftTileCoords	= playerTileCoords + IntVec2( -1, 0 );
	IntVec2 rightTileCoords	= playerTileCoords + IntVec2( 1, 0 );
	
	IntVec2 leftDownTileCoords	= playerTileCoords + IntVec2( -1, -1 );
	IntVec2 leftUpTileCoords	= playerTileCoords + IntVec2( -1, 1 );
	IntVec2 rightDownTileCoords	= playerTileCoords + IntVec2( 1, -1 );
	IntVec2 rightUpTileCoords	= playerTileCoords + IntVec2( 1, 1 );

	CheckActorTileCollisionWithTileCoords( m_player, downTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, upTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, leftTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, rightTileCoords );

	CheckActorTileCollisionWithTileCoords( m_player, leftDownTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, leftUpTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, rightDownTileCoords );
	CheckActorTileCollisionWithTileCoords( m_player, rightUpTileCoords );
}

void Map::CheckActorTileCollisionWithTileCoords( Actor* actor, IntVec2 tileCoords )
{
	if( !IsTileCoordsValid( tileCoords ) ){ return; }
	if( !IsTileSolid( tileCoords ) ){ return; }
	Player* player = dynamic_cast<Player*>(actor);
	if( player ) {
		if( player->GetHasKey() && IsTileDoor( tileCoords ) ){ return; }
	}

	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	Tile const& tempTile = m_tiles[tileIndex];
	AABB2 tileBound = tempTile.GetTileBounds();
	Vec2 actorPos = actor->GetPosition(); 
	PushDiscOutOfAABB2D( actorPos, actor->GetPhysicRadius(), tileBound );
	actor->SetPosition( actorPos );
}

void Map::CheckPlayerOnLava()
{
	Vec2 playerPos = m_player->GetPosition();
	IntVec2 playerTileCoords = IntVec2( (int)playerPos.x, (int)playerPos.y );
	if( IsTileOfTypeWithCoords( "Lava", playerTileCoords ) ) {
		m_player->SetIsOnLava( true );
	}
	else {
		m_player->SetIsOnLava( false );
	}
}

void Map::CheckFight()
{
	if( m_isFighting ){
		if( m_player->GetHealth() <= 0  ) {
			// End Game;
			g_theGame->SetGameEnd();
			EndFight();
		}
		else if( m_fightEnemy->GetHealth() <= 0 ) {
			EventArgs args;
			std::string resultString = m_fightEnemy->GetType() + " is defeated.";
			args.SetValue( "message", resultString );
			g_theEventSystem->FireEvent( "DebugMessage", args );
			DeleteEnemy( m_fightEnemy );
			m_fightEnemy = nullptr;
			EndFight();
		}
	}
	else {
		for( int i = 0; i < m_enemies.size(); i++ ) {
		 	Vec2 playerPos = m_player->GetPosition();
			Actor* tempEnemy = m_enemies[i];
			Vec2 enemyPos = tempEnemy->GetPosition();
			Vec3 disp = playerPos - enemyPos;
// 			float test1 =  disp.GetLengthSquared();
// 			float test2 = (m_player->GetPhysicRadius() * m_player->GetPhysicRadius()) + (tempEnemy->GetPhysicRadius() * tempEnemy->GetPhysicRadius());
			if( disp.GetLengthSquared() < (m_player->GetPhysicRadius() * m_player->GetPhysicRadius()) + (tempEnemy->GetPhysicRadius() * tempEnemy->GetPhysicRadius()) ) {
				StartFight( tempEnemy );
				break;
			}
		}
	}
}

void Map::StartFight( Actor* enemy )
{
	m_isFighting = true;
	m_player->SetDisableInput( true );
	m_player->SetDisableMove( true );
	if( m_fightTimer == nullptr ) {
		m_fightTimer = new Timer();
	}
	m_fightTimer->SetSeconds( m_fightDeltaSecondsEachTurn );
	m_fightEnemy = enemy;
	m_fightVerts.clear();
	m_fightBox = AABB2( m_player->GetPosition() - Vec2( 0.5f ), m_player->GetPosition() + Vec2( 0.5f ) );
}

void Map::UpdateFight( Actor* enemy, float deltaSeconds )
{
	static bool isPlayerAttack = true;
	if( m_fightTimer->CheckAndDecrement() ) {
		isPlayerAttack = !isPlayerAttack;
		
		if( isPlayerAttack ) {
			enemy->TakeDamage( m_player->GetAttackStrength() ); 
		}
		else {
			m_player->TakeDamage( enemy->GetAttackStrength() );
		}
		SoundID fightSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/SwordHit.mp3" );
		g_theAudioSystem->PlaySound( fightSound, false, g_theGame->m_volume );
		UpdateFightAnimation( deltaSeconds );
	}
}

void Map::EndFight()
{
	m_isFighting = false;
	m_player->SetDisableInput( false );
	m_player->SetDisableMove( false );
	m_fightEnemy = nullptr;
}

void Map::UpdateFightAnimation( float deltaSeconds )
{
	static float totalSeconds = 0.f;
	totalSeconds += deltaSeconds;
	const SpriteDefinition& currentSpriteDef = m_fightAnimDef->GetSpriteDefAtTime( totalSeconds );
	Vec2 uvAtMin;
	Vec2 uvAtMax;
	m_fightTexture = m_fightAnimDef->GetSpriteSheet().GetTexture();
	currentSpriteDef.GetUVs( uvAtMin, uvAtMax );
	m_fightVerts.clear();
	AppendVertsForAABB2D( m_fightVerts, m_fightBox, Rgba8::WHITE, uvAtMin, uvAtMax );
}

void Map::RenderFightAnimation()
{
	g_theRenderer->SetDiffuseTexture( m_fightTexture );
	g_theRenderer->DrawVertexVector( m_fightVerts );
	g_theRenderer->SetDiffuseTexture( nullptr );
}

bool Map::GetIsFighting() const
{
	return m_isFighting;
}

const Actor* Map::GetFightEnemy() const
{
	return m_fightEnemy;
}

void Map::DebugDrawTiles()
{
	m_debugTilesVertices.clear();
	m_debugTilesVertices.reserve( m_tiles.size() * 6 );
	for( int i = 0; i < m_tiles.size(); i++ ) {
		Tile tempTile = m_tiles[i];
		AABB2 tempBound = tempTile.GetTileBounds();
		TileDefinition* tempTileDef = tempTile.GetTileDef();
		AppendVertsForAABB2D( m_debugTilesVertices, tempBound, tempTileDef->GetTintColor(), tempTileDef->GetSpriteUVAtMins(), tempTileDef->GetSpriteUVAtMaxs() );
	}
	g_gameCamera->EnableClearColor( Rgba8::RED );
	g_theRenderer->BeginCamera( g_gameCamera );
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->SetDiffuseTexture( tileTexture );
	g_theRenderer->DrawVertexVector( m_debugTilesVertices );
	g_theRenderer->EndCamera( );
	g_theRenderer->Prensent();
}