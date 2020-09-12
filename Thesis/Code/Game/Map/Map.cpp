#include "Map.hpp"
#include <stack>
#include <time.h>
#include "Game/Player.hpp"
#include "Game/Item.hpp"
#include "Game/Game.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/MapGenStep.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Game/Camera/CameraSystem.hpp"
#include "Game/Projectile.hpp"

#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Platform/Window.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//////////////////////////////////////////////////////////////////////////
// Global variable
//////////////////////////////////////////////////////////////////////////

// Game
//////////////////////////////////////////////////////////////////////////
extern Game*			g_theGame;
extern Camera*			g_debugCamera;
extern Camera*			g_gameCamera;
extern CameraSystem*	g_theCameraSystem;

// Engine
//////////////////////////////////////////////////////////////////////////
extern AudioSystem*		g_theAudioSystem;
extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;
extern Window*			g_theWindow;
extern DevConsole*		g_theConsole;
//////////////////////////////////////////////////////////////////////////


Map::Map( std::string name, MapDefinition* definition )
	:m_name(name)
	,m_definition(definition)
{
	static int seed = 0;
	m_rng = new RandomNumberGenerator( seed );
	m_width = m_definition->GetWidth();
	m_height = m_definition->GetHeight();
	m_tiles.reserve( m_width * m_height );
	m_tileAttributes.reserve( m_width * m_height );
	seed++;
}

Map* Map::CreateMap( std::string name, MapDefinition* definition )
{
	Map* newMap = new Map( name, definition );
	return newMap;
}

// Accessor
//////////////////////////////////////////////////////////////////////////
bool Map::IsTileOfTypeInsideWithCoords( TileType type, IntVec2 tileCoords ) const
{
	return true;
}

bool Map::IsTileOfTypeWithIndex( TileType type, int index ) const
{
	return true;
}

bool Map::IsTileCoordsInside( IntVec2 tileCoords ) const
{
	return ( 0 < tileCoords.x && tileCoords.x < m_width ) && ( 0 < tileCoords.y && tileCoords.y < m_height );
}

bool Map::IsTileOfAttribute( IntVec2 tileCoords, TileAttribute attr ) const
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	uint tileAttributeFlags = m_tileAttributes[tileIndex];
	TileAttributeBitFlag attrBitFlag = GetTileAttrBitFlagWithTileAttr( attr );
	return ( ( tileAttributeFlags & attrBitFlag ) == attrBitFlag );
}

bool Map::IsTileOfAttribute( int tileIndex, TileAttribute attr ) const
{
	uint tileAttributeFlags = m_tileAttributes[tileIndex];
	TileAttributeBitFlag attrBitFlag = GetTileAttrBitFlagWithTileAttr( attr );
	return (( tileAttributeFlags & attrBitFlag ) == attrBitFlag );
}

bool Map::IsTileSolid( IntVec2 tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_SOLID );
}

bool Map::IsTileRoom( IntVec2 tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_ROOM );
}

bool Map::IsTileEdge( IntVec2 tileCoords ) const
{
	return ( tileCoords.x == 0 && tileCoords.x == ( m_width - 1 )) && ( tileCoords.y == 0 && tileCoords.y == ( m_height - 1));
}

bool Map::IsTileVisited( IntVec2 tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_VISITED );
}

bool Map::IsTilesSameRoom( IntVec2 tileCoords1, IntVec2 tileCoords2 )
{
	for( int i = 0; i < m_rooms.size(); i++ ) {
		Room* tempRoom = m_rooms[i];
		if( tempRoom->IsTileOfRoom( tileCoords2 ) && tempRoom->IsTileOfRoom( tileCoords1 ) ) {
			return true;
		}
	}

	return false;
}

bool Map::IsTilesSameRoomFloor( IntVec2 tileCoords1, IntVec2 tileCoords2 )
{
	for( int i = 0; i < m_rooms.size(); i++ ) {
		Room* tempRoom = m_rooms[i];
		if( tempRoom->IsTileOfRoomFloor( tileCoords2 ) && tempRoom->IsTileOfRoomFloor( tileCoords1 ) ) {
			return true;
		}
	}

	return false;
}

bool Map::IsTileCoordsValid( IntVec2 tileCoords )
{
	return ( tileCoords.x >= 0 && tileCoords.x < m_width ) && ( tileCoords.y >= 0 && tileCoords.y < m_height );
}

IntVec2 Map::GetTileCoordsWithTileIndex( int index ) const
{
	IntVec2 tileCoords;
	tileCoords.x = index % m_width;
	tileCoords.y = index / m_width;
	return tileCoords;
}

IntVec2 Map::GetRandomInsideTileCoords() const
{
	IntVec2 tileCoords;
	tileCoords.x = m_rng->RollRandomIntInRange( 1, m_width - 1 );
	tileCoords.y = m_rng->RollRandomIntInRange( 1, m_height - 1 );
	return tileCoords;
}

IntVec2 Map::GetRandomInsideNotSolidTileCoords() const
{
	while( true ) {
		IntVec2 tileCoords = GetRandomInsideTileCoords();
		if( !IsTileSolid( tileCoords ) ) {
			return tileCoords;
		}
	}
}

int Map::GetTileIndexWithTileCoords( IntVec2 tileCoords ) const
{
	return m_width * tileCoords.y + tileCoords.x;
}

const Tile& Map::GetTileWithTileCoords( IntVec2 coords )
{
	int tileIndex = GetTileIndexWithTileCoords( coords );
	return m_tiles[tileIndex];
}
//////////////////////////////////////////////////////////////////////////


// Mutator
//////////////////////////////////////////////////////////////////////////
void Map::SetTileTypeWithCoords( IntVec2 coords, TileType type )
{
	int tileIndex = GetTileIndexWithTileCoords( coords );
	SetTileTypeWithIndex( tileIndex, type );
}

void Map::SetTileTypeWithIndex( int index, TileType type )
{
	m_tiles[index].SetTileType( type );
}

void Map::SetTileOfAttribute( IntVec2 coords, TileAttribute attr, bool isTrue )
{
	int tileIndex = GetTileIndexWithTileCoords( coords );
	TileAttributeBitFlag attrBitFlag = GetTileAttrBitFlagWithTileAttr( attr );
	if( isTrue ) {
		m_tileAttributes[tileIndex] = m_tileAttributes[tileIndex] | attrBitFlag;	 
	}
	else {
		m_tileAttributes[tileIndex] = m_tileAttributes[tileIndex] & ~attrBitFlag;
	}
}

void Map::SetTileSolid( IntVec2 coords, bool isSolid )
{
	SetTileOfAttribute( coords, TILE_IS_SOLID, isSolid );
}

void Map::SetTileRoom( IntVec2 coords, bool isRoom )
{
	SetTileOfAttribute( coords, TILE_IS_ROOM, isRoom );
}

void Map::AddRoom( Room* room )
{
	m_rooms.push_back( room );
}

//////////////////////////////////////////////////////////////////////////


// Basic
//////////////////////////////////////////////////////////////////////////
void Map::UpdateMap( float deltaSeconds )
{
	// generate map
	// update map
	if( m_isGeneratingMap ) {
		m_tilesVertices.clear();
		// initialize
		// Generate rooms
		// Eliminate dead end
		static int stepIndex = 0;
		switch( stepIndex )
		{
		case 0:
			InitializeTiles();
			PopulateTiles();
			stepIndex++;
			break;
		case 1:
			GenerateRooms();
			PopulateTiles();
			stepIndex++;
			break;	
		case 2:
			m_startCoords = GetRandomInsideNotSolidTileCoords();
			PopulateTiles();
			m_isGeneratingMap = false;
		default:
			break;
		}
		
	}

	UpdateMouse();
	UpdateActors( deltaSeconds );
	UpdateProjectiles( deltaSeconds );
	CheckCollision();

	if( g_theGame->GetIsDebug() ) {
		UpdateDebugInfo();
	}
}

void Map::UpdateActors( float deltaSeconds )
{
	if( m_players.size() == 1 ) {
		Vec2 playerPos = m_players[0]->GetPosition();
		Vec2 targetDirt = m_mousePosInWorld - playerPos;
		m_players[0]->SetOrientationDegrees( targetDirt.GetAngleDegrees() );
	}
	for( int i = 0; i < m_players.size(); i++ ) {
		m_players[i]->UpdatePlayer( deltaSeconds );
	}
}

void Map::UpdateProjectiles( float deltaSeconds )
{
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		m_projectiles[i]->UpdateProjectile( deltaSeconds );
	}
}

void Map::RenderMap()
{
	g_theRenderer->SetDiffuseTexture( TileDefinition::s_spriteTexture );
	g_theRenderer->DrawVertexVector( m_tilesVertices );

	RenderActors();
	RenderProjectiles();

	if( g_theGame->GetIsDebug() ) {
		RenderDebugInfo();
	}
}

void Map::RenderActors()
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int i = 0; i < m_players.size(); i++ ) {
		m_players[i]->RenderPlayer();
	}
}

void Map::RenderProjectiles()
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		m_projectiles[i]->RenderProjectile();
	}
}

void Map::CreatePlayer( )
{
	Vec2 pos = (Vec2)m_startCoords;
	Player* newPlayer = Player::SpawnPlayerWithPos( pos );
	newPlayer->SetMap( this );
	m_players.push_back( newPlayer );
	g_theCameraSystem->CreateAndPushController( newPlayer, g_gameCamera );
}

void Map::SpawnNewProjectile( ActorType type, Vec2 startPos, Vec2 movingDirt )
{
	Projectile* tempProjectile = Projectile::SpawnProjectileWithDirtAndType( movingDirt, startPos, type );
	m_projectiles.push_back( tempProjectile );
}

void Map::CheckCollision()
{
	CheckActorCollision();
	CheckTileCollision();
	CheckBulletCollision();
}

void Map::CheckActorCollision()
{

}

void Map::CheckTileCollision()
{
	for( int i = 0; i < m_players.size(); i++ ) {
		Player* tempPlayer = m_players[i];
		if( tempPlayer == nullptr ){ continue; }
		CheckCollisionBetweenPlayerAndTiles( tempPlayer );
	}
}

void Map::CheckBulletCollision()
{
	
}

void Map::CheckCollisionBetweenPlayerAndTiles( Player* player )
{
	IntVec2 playerTileCoords = (IntVec2)player->GetPosition(); 

	IntVec2 downTileCoords	= playerTileCoords + IntVec2( 0, -1 );
	IntVec2 upTileCoords	= playerTileCoords + IntVec2( 0, 1 );
	IntVec2 rightTileCoords	= playerTileCoords + IntVec2( 1, 0 );
	IntVec2 leftTileCoords	= playerTileCoords + IntVec2( -1, 0 );

	IntVec2 downLeftTileCoords	= playerTileCoords + IntVec2( -1, -1 );
	IntVec2 downRightTileCoords	= playerTileCoords + IntVec2( 1, -1 );
	IntVec2 upLeftTileCoords	= playerTileCoords + IntVec2( -1, 1 );
	IntVec2 upRightTileCoords	= playerTileCoords + IntVec2( 1, 1 );

	CheckCollisionBetweenPlayerAndTile( player, downTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, upTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, rightTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, leftTileCoords );

	CheckCollisionBetweenPlayerAndTile( player, downLeftTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, downRightTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, upLeftTileCoords );
	CheckCollisionBetweenPlayerAndTile( player, upRightTileCoords );
}

void Map::CheckCollisionBetweenPlayerAndTile( Player* player, IntVec2 tileCoords )
{
	if( !IsTileCoordsValid( tileCoords ) ) { 
		ERROR_RECOVERABLE( "tileCoords not valid !" );
		return; 
	}

	if( !IsTileSolid( tileCoords ) ){ return; }

	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	Tile tempTile = m_tiles[tileIndex];
	AABB2 tileBound = tempTile.GetTileBounds();
	Vec2 playerPos = player->GetPosition();
	PushDiscOutOfAABB2D( playerPos, player->GetPhysicRadius(), tileBound );
	player->SetPosition( playerPos );
}

//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////

// Generate map
//////////////////////////////////////////////////////////////////////////
void Map::LoadMapDefinitions()
{
	m_width		= m_definition->GetWidth();
	m_height	= m_definition->GetHeight();
	
	m_tiles.clear();
	m_tiles.reserve( m_width * m_height );
}

void Map::PopulateTiles()
{
	m_tilesVertices.clear();
	m_tilesVertices.reserve( m_tiles.size() * 4 );
	// TODO debuging
	for( int i = 0; i < m_tiles.size(); i++ ) {
		Tile& tempTile = m_tiles[i];
		TileDefinition* tempTileDef = tempTile.GetTileDef();
		AABB2 tileBounds = tempTile.GetTileBounds();
		Vec2 tileUVAtMin = tempTileDef->GetSpriteUVAtMins();
		Vec2 tileUVAtMax = tempTileDef->GetSpriteUVAtMaxs();

		AppendVertsForAABB2D( m_tilesVertices, tileBounds, Rgba8::WHITE, tileUVAtMin, tileUVAtMax );
	}
}

void Map::InitializeTiles()
{
	for( int i = 0; i < m_height * m_width; i++ ) {
		IntVec2 tileCoords = GetTileCoordsWithTileIndex( i );
		m_tiles.emplace_back( tileCoords, m_definition->GetFillType() );
		m_tileAttributes.push_back( (uint)0 );
	}
}

void Map::GenerateRooms()
{
	const std::vector<MapGenStep*>& mapSteps = m_definition->GetMapGenSteps();
	for( int i = 0; i < mapSteps.size(); i++ ) {
		mapSteps[i]->RunStep( this );
	}
}

void Map::GeneratePaths()
{
	// check if able to create maze
	// find available tiles
	// use DFS explore mazes.
	// If end, keep find available and explore until no available tiles
	//while
	// 
	while( true ) {
		IntVec2 startTileCoords;
		while( true ) {
			startTileCoords = GetRandomInsideTileCoords();
			if( IsTileSolid( startTileCoords ) && !IsTileRoom( startTileCoords ) ) {
				break;
			}
		}

		for( int i = 0; i < m_rooms.size(); i++ ) {
			IntVec2 doorCoords1 = m_rooms[i]->GetRandomNearestEdgeTileCoord( startTileCoords );
			IntVec2 doorCoords2 = m_rooms[i]->GetRandomEdgeTileCoord();
			CreatePathBetweenCoords( startTileCoords, doorCoords1 );
			startTileCoords = doorCoords2;
		}
		break;
	}
}

void Map::CreatePathBetweenCoords( IntVec2 coord1, IntVec2 coord2 )
{
	IntVec2 tempCoords = coord1;
	while( tempCoords != coord2 && !IsTilesSameRoomFloor( tempCoords, coord2 ) ) {
		SetTileTypeWithCoords( tempCoords, m_definition->GetFillType() );
		IntVec2 disp = coord2 - tempCoords;
		if( disp.x > 0 ) { 
			tempCoords = tempCoords + IntVec2( 1, 0 );
			continue;
		}
		else if( disp.x < 0 ) {
			tempCoords = tempCoords + IntVec2( -1, 0 );
			continue;
		}
		else if( disp.y > 0 ) {
			tempCoords = tempCoords + IntVec2( 0, 1 );
			continue;
		}
		else if( disp.y < 0 ) {
			tempCoords = tempCoords + IntVec2( 0, -1 );
			continue;
		}
	}
}
//////////////////////////////////////////////////////////////////////////


// Private Update
//////////////////////////////////////////////////////////////////////////
void Map::UpdateDebugInfo()
{
	// Get cursor info
	// Get debug tile
	// Set Debug Info
	IntVec2 tileCoords = IntVec2( m_mousePosInWorld );
	m_debugTileIndex = GetTileIndexWithTileCoords( tileCoords );
	//g_theConsole->DebugLogf( " tileindex = %d", tileIndex );
}

void Map::UpdateMouse()
{
	HWND handle = (HWND)g_theWindow->GetHandle();
	Vec2 mousePosInClient = g_theInputSystem->GetNormalizedMousePosInClient( handle );
	m_mousePosInWorld = (Vec2)g_gameCamera->ClientToWorld( mousePosInClient, 1.f );
	//g_theConsole->DebugLog( m_mousePosInWorld.ToString(), Rgba8::WHITE );
}
//////////////////////////////////////////////////////////////////////////

// Private Render
//////////////////////////////////////////////////////////////////////////
void Map::RenderDebugInfo()
{
	RenderDebugTile( m_debugTileIndex );
}

void Map::RenderDebugTile( int tileIndex )
{
	if( tileIndex < 0 || tileIndex >= m_tiles.size() ){ return; }
	int lineIndex = 0;
	Tile debugTile = m_tiles[tileIndex];
	TileType debugTiletype = debugTile.GetTileType();
	Vec2 tileCoords = debugTile.GetWorldPos();
	DebugAddScreenLeftAlignText( 0, 3.f * lineIndex, Vec2::ZERO, Rgba8::WHITE, 0.f, debugTiletype );
	lineIndex++;
	DebugAddScreenLeftAlignText( 0, 3.f * lineIndex, Vec2::ZERO, Rgba8::WHITE, 0.f, tileCoords.ToString() );
	lineIndex++;
	std::string isStartText = GetStringFromBool( IsTileOfAttribute( tileIndex, TILE_IS_START ) );
	DebugAddScreenLeftAlignTextf( 0, 3.f * lineIndex, Vec2::ZERO, Rgba8::WHITE, "isStart = %s", isStartText.c_str() );
	lineIndex++;
	std::string isRoomText = GetStringFromBool( IsTileOfAttribute( tileIndex, TILE_IS_ROOM ) );
	DebugAddScreenLeftAlignTextf( 0, 3.f * lineIndex, Vec2::ZERO, Rgba8::WHITE, "isRoom = %s", isRoomText.c_str() );
	lineIndex++;
	std::string isSolidText = GetStringFromBool( IsTileOfAttribute( tileIndex, TILE_IS_SOLID ) );
	DebugAddScreenLeftAlignTextf( 0, 3.f * lineIndex, Vec2::ZERO, Rgba8::WHITE, "isSolid = %s", isSolidText.c_str() );
// 	lineIndex++;
// 	std::string isStartText = GetStringFromBool( IsTileOfAttribute( tileIndex, TILE_IS_START ) );
// 	DebugAddScreenLeftAlignTextf( 0, 3 * lineIndex, Vec2::ZERO, Rgba8::WHITE, "isStart = %s", isStartText.c_str() );
}

TileAttributeBitFlag Map::GetTileAttrBitFlagWithTileAttr( TileAttribute attr ) const
{
	switch( attr )
	{
	case TILE_IS_START:		return TILE_IS_START_BIT;
	case TILE_IS_EXIT:		return TILE_IS_EXIT_BIT;
	case TILE_IS_ROOM:		return TILE_IS_ROOM_BIT;
	case TILE_IS_SOLID:		return TILE_IS_SOLID_BIT;
	case TILE_IS_VISITED:	return TILE_IS_VISITED_BIT;
	case TILE_IS_DOOR:		return TILE_IS_DOOR_BIT;
	default:
		ERROR_RECOVERABLE( "no attribute!" );
	}
	return TILE_NON_ATTR_BIT;
}
//////////////////////////////////////////////////////////////////////////
