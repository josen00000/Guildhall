#include "Map.hpp"
#include <stack>
#include <time.h>
#include "Game/Player.hpp"
#include "Game/Item.hpp"
#include "Game/Game.hpp"
#include "Game/Enemy.hpp"
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
	newMap->GenerateMap();
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
	UNUSED( index );
	return true;
}

bool Map::IsTileCoordsInside( IntVec2 const&  tileCoords ) const
{
	return ( 0 < tileCoords.x && tileCoords.x < m_width ) && ( 0 < tileCoords.y && tileCoords.y < m_height );
}

bool Map::IsTileOfAttribute( IntVec2 const& tileCoords, TileAttribute attr ) const
{
	if( !IsTileCoordsValid( tileCoords ) ){ return false; }
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

bool Map::IsTileSolid( IntVec2 const& tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_SOLID );
}

bool Map::IsTileRoom( IntVec2 const& tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_ROOM );
}

bool Map::IsTileEdge( IntVec2 const& tileCoords ) const
{
	return ( tileCoords.x == 0 && tileCoords.x == ( m_width - 1 )) && ( tileCoords.y == 0 && tileCoords.y == ( m_height - 1));
}

bool Map::IsTileWalkable( IntVec2 const& tileCoords ) const
{
	return IsTileOfAttribute( tileCoords, TILE_IS_WALKABLE );
}

bool Map::IsTilesSameRoom( IntVec2 const& tileCoords1, IntVec2 const& tileCoords2 )
{
	for( int i = 0; i < m_rooms.size(); i++ ) {
		Room* tempRoom = m_rooms[i];
		if( tempRoom->IsTileOfRoom( tileCoords2 ) && tempRoom->IsTileOfRoom( tileCoords1 ) ) {
			return true;
		}
	}

	return false;
}

bool Map::IsTilesSameRoomFloor( IntVec2 const& tileCoords1, IntVec2 const& tileCoords2 )
{
	for( int i = 0; i < m_rooms.size(); i++ ) {
		Room* tempRoom = m_rooms[i];
		if( tempRoom->IsTileOfRoomFloor( tileCoords2 ) && tempRoom->IsTileOfRoomFloor( tileCoords1 ) ) {
			return true;
		}
	}

	return false;
}

bool Map::IsTileCoordsValid( IntVec2 const& tileCoords ) const
{
	return ( tileCoords.x >= 0 && tileCoords.x < m_width ) && ( tileCoords.y >= 0 && tileCoords.y < m_height );
}

bool Map::IsPosNotSolid( Vec2 pos )
{
	IntVec2 tileCoords = IntVec2( pos );
	return !IsTileSolid( tileCoords );
}

Vec2 Map::GetPlayerPosWithIndex( int index )
{
	if( m_players.size() < index ) {
		g_theConsole->DebugErrorf( "Index: %d does not exist in get player pos.", index );
	}
	return m_players[index]->GetPosition();
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

IntVec2 Map::GetRandomInsideCameraNotSolidTileCoords( Camera* camera ) const
{
	while ( true )
	{
		IntVec2 result = GetRandomInsideNotSolidTileCoords();
		AABB2 cameraBox = camera->GetWorldBox();
		if( cameraBox.IsPointInside( (Vec2)result )){ return result; }
	}
}

Vec2 Map::GetCuePos( int index ) const
{
	if( m_name == "level2" ) {
		Vec2 playerPos = m_players[index]->GetPosition();
		if( m_isFirstDoorLocked ) {
			if( playerPos.x >=0 && playerPos.x <= 40 ) {
				return (Vec2)m_triggerTiles[0];
			}
			else if( playerPos.x >= 40 && playerPos.x <= 80 ) {
				return (Vec2)m_triggerTiles[1];
			}
			else {
				return Vec2::ZERO;
			}
		}
		else {
			if( GetDistance2D( playerPos, Vec2( m_triggerTiles[0] ) ) < 2.f || GetDistance2D( playerPos, Vec2( m_triggerTiles[1] ) ) < 2.f ) {
				return playerPos;
			}
			else {
				return Vec2( m_doorTiles[0] );
			}
		}
	}
	else{
		if( m_activeItem == nullptr ) {
			return Vec2::ZERO;
		}
		return m_activeItem->GetPosition();
	}
}

Vec2 Map::GetRandomEnemyPos() const
{
	IntVec2 tileCoords;
	while ( true )
	{
		tileCoords.x = m_rng->RollRandomIntInRange( 40, 80 );
		tileCoords.y = m_rng->RollRandomIntInRange( 1, m_height - 1 );
		if( !IsTileSolid( tileCoords ) ) {
			return (Vec2)tileCoords;
		}
	}
}

Player* Map::GetPlayerWithIndex( int index )
{
	if( m_players.size() < index ) {
		g_theConsole->DebugErrorf( "Index: %d does not exist in get player pos.", index );
	}

	return m_players[index];
}

std::vector<Enemy*>& Map::GetEnemies()
{
	return m_enemies;
}

std::vector<Player*>& Map::GetPlayers()
{
	return m_players;
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
void Map::SetTileTypeWithCoords( IntVec2 const& coords, TileType type )
{
	int tileIndex = GetTileIndexWithTileCoords( coords );
	SetTileTypeWithIndex( tileIndex, type );
}

void Map::SetTileTypeWithIndex( int index, TileType type )
{
	m_tiles[index].SetTileType( type );
}

void Map::SetTileOfAttribute( IntVec2 const& coords, TileAttribute attr, bool isTrue )
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

void Map::SetTileSolid( IntVec2 const& coords, bool isSolid )
{
	SetTileOfAttribute( coords, TILE_IS_SOLID, isSolid );
}

void Map::SetTileWalkable( IntVec2 const& coords, bool isWalkable )
{
	SetTileOfAttribute( coords, TILE_IS_WALKABLE, isWalkable );
}

void Map::SetTileRoom( IntVec2 const& coords, bool isRoom )
{
	SetTileOfAttribute( coords, TILE_IS_ROOM, isRoom );
}

void Map::SetStartcoords( IntVec2 const& coords )
{
	m_startCoords = coords;
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
	UpdateMouse();
	UpdateFirstLockedDoorsTiles();
	UpdateSecondLockedDoorsTiles();
	CheckNeedGenerateEnemies();
	CheckNeedGenerateBoss();
	CheckPlayerStates();
	UpdateActors( deltaSeconds );
	UpdateProjectiles( deltaSeconds );
	CheckCollision();
	if( m_boss ) {
		m_boss->UpdateBoss( deltaSeconds );
	}
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
		if( m_players[i] == nullptr ){ continue;}
		m_players[i]->UpdatePlayer( deltaSeconds, i );
	}

	for( int i = 0; i < m_enemies.size(); i++ ) {
		if( m_enemies[i] == nullptr ){ continue;}
		m_enemies[i]->UpdateEnemy( deltaSeconds );
	}
}

void Map::UpdateProjectiles( float deltaSeconds )
{
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		if( m_projectiles[i] == nullptr ){ continue;}
		m_projectiles[i]->UpdateProjectile( deltaSeconds );
	}
}

void Map::UpdateItems( float deltaSeconds )
{
	UNUSED( deltaSeconds );
}

void Map::UpdateFirstLockedDoorsTiles()
{
	if( m_name != "level2" ){ return; }
	bool shouldFirstDoorLocked = true;
	// first door
	TileType openDoorType	= m_definition->GetOpenType();
	TileType lockedDoorType = m_definition->GetLockedType();
	for( int i = 0; i < m_players.size(); i++ ) {
		IntVec2 playerPos = (IntVec2)m_players[i]->GetPosition();
		for( int j = 0; j < m_triggerTiles.size(); j++ ) {
			float distance = GetIntDistance2D( playerPos, m_triggerTiles[j] );
			if( distance < 2 ) {
				shouldFirstDoorLocked = false;
				break;
			}
		}
		if( !shouldFirstDoorLocked ) {
			break;
		}
	}

	if( shouldFirstDoorLocked == m_isFirstDoorLocked ) {
		return;
	}
	else if ( shouldFirstDoorLocked ) {
		SetTileTypeWithCoords( m_doorTiles[0], lockedDoorType );
		SetTileTypeWithCoords( m_doorTiles[1], lockedDoorType );
		SetTileSolid( m_doorTiles[0], true );
		SetTileSolid( m_doorTiles[1], true );
		PopulateTiles();
	}
	else if( !shouldFirstDoorLocked ) {
		SetTileTypeWithCoords( m_doorTiles[0], openDoorType );
		SetTileTypeWithCoords( m_doorTiles[1], openDoorType );
		SetTileSolid( m_doorTiles[0], false );
		SetTileSolid( m_doorTiles[1], false );
		PopulateTiles();
	}
	else {}
	m_isFirstDoorLocked = shouldFirstDoorLocked;
}

void Map::UpdateSecondLockedDoorsTiles()
{
	if( m_name != "level2" ){ return; }
	bool shouldSecondDoorLocked = true;
	TileType openDoorType	= m_definition->GetOpenType();
	TileType lockedDoorType = m_definition->GetLockedType();
	bool doesAliveEnemyExist = false;
	for( Enemy* enemy : m_enemies ) {
		if( enemy ) {
			doesAliveEnemyExist = true;
			break;
		}
	}
	if( !doesAliveEnemyExist && m_isEnemiesGenerated ) {
		shouldSecondDoorLocked = false;
	}
	if( m_isSecondDoorLocked == shouldSecondDoorLocked ){ return; }
	else if( shouldSecondDoorLocked ) {
		SetTileTypeWithCoords( m_doorTiles[2], lockedDoorType );
		SetTileTypeWithCoords( m_doorTiles[3], lockedDoorType );
		SetTileSolid( m_doorTiles[2], true );
		SetTileSolid( m_doorTiles[3], true );
		PopulateTiles();
	}
	else {
		SetTileTypeWithCoords( m_doorTiles[2], openDoorType );
		SetTileTypeWithCoords( m_doorTiles[3], openDoorType );
		SetTileSolid( m_doorTiles[2], false );
		SetTileSolid( m_doorTiles[3], false );
		PopulateTiles();
	}
	m_isSecondDoorLocked = shouldSecondDoorLocked;
}

void Map::RenderMap()
{
	g_theRenderer->SetDiffuseTexture( TileDefinition::s_spriteTexture );
	g_theRenderer->DrawVertexVector( m_tilesVertices );

	RenderActors();
	RenderProjectiles();
	RenderItems();
	if( m_boss ) {
		m_boss->RenderBoss();
	}

	if( g_theGame->GetIsDebug() ) {
		//RenderDebugInfo();
	}
}

void Map::RenderActors()
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int i = 0; i < m_players.size(); i++ ) {
		if( m_players[i]->GetAliveState() != ALIVE ){ continue; }
		m_players[i]->RenderPlayer();
	}
	for( int i = 0; i < m_enemies.size(); i++ ) {
		if( !m_enemies[i] ){ continue; }
		if( m_enemies[i]->GetAliveState()  != ALIVE ){ continue; }
		m_enemies[i]->RenderEnemy();
	}
}

void Map::RenderProjectiles()
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		if( m_projectiles[i] == nullptr ){ continue; }
		m_projectiles[i]->RenderProjectile();
	}
}

void Map::RenderItems()
{
	for( int i = 0; i < m_items.size(); i++ ) {
		m_items[i]->RenderItem();
	}
}

void Map::EndFrame()
{
	ManageGarbage();	
}

void Map::GenerateMap()
{
	m_tilesVertices.clear();
	InitializeTiles();
	RunMapGenSteps();
	GenerateEdges();
	InitializeMapDifferentTiles();
	PopulateTiles();
}

void Map::CreatePlayer( )
{
	if( m_players.size() >= 4 ){ return; }
		Vec2 pos = (Vec2)m_startCoords;
	if( m_name == "level1" ) {
		 pos = (Vec2)GetRandomInsideCameraNotSolidTileCoords( g_theGame->m_gameCamera );
	}
	Player* newPlayer = Player::SpawnPlayerWithPos( pos, (int)m_players.size() );
	newPlayer->SetMap( this );
	m_players.push_back( newPlayer );
	g_theCameraSystem->CreateAndPushController( newPlayer );
}

void Map::DestroyPlayerWithIndex( int index )
{
	if( index >= m_players.size() ){ 
		g_theConsole->DebugErrorf( "destroy player out of range: %d", index );
		return; 
	}
	m_players[index]->SetAliveState( WAIT_FOR_DELETE );
	g_theCameraSystem->PrepareRemoveAndDestroyController( m_players[index] );
	
}

void Map::DestroyAllPlayers()
{
	for( int i = 0; i < m_players.size(); i++ ) {
		delete m_players[i];
	}
	g_theCameraSystem->DestroyAllControllers();
}

void Map::ShiftPlayer()
{
	Player* keyboardPlayer	= m_players[0];
	for( int i = 0; i < m_players.size(); i++ ) {
		Player* tempPlayer = m_players[i];
		if( tempPlayer->IsControlledByUser() ) { continue; }
		
		m_players[0] = tempPlayer;
		m_players[i] = nullptr;
		m_players.erase( m_players.begin() + i );
		break;
	}
	
	m_players.push_back( keyboardPlayer );
}

void Map::SpawnNewEnemy( Vec2 startPos )
{
	Enemy* tempEnemy = Enemy::SpawnPlayerWithPos( this, startPos );
	m_enemies.push_back( tempEnemy );
}

void Map::SpawnNewProjectile( ActorType type, Vec2 startPos, Vec2 movingDirt, Rgba8 color )
{
	Projectile* tempProjectile = Projectile::SpawnProjectileWithDirtAndType( movingDirt, startPos, type );
	tempProjectile->SetColor( color );
	m_projectiles.push_back( tempProjectile );
}

void Map::SpawnNewProjectileWithDamage( ActorType type, Vec2 startPos, Vec2 movingDirt, Rgba8 color, float damage )
{
	Projectile* tempProjectile = Projectile::SpawnProjectileWithDirtAndType( movingDirt, startPos, type );
	tempProjectile->SetDamage( damage );
	tempProjectile->SetColor( color );
	m_projectiles.push_back( tempProjectile );
}

void Map::SpawnNewItem( Vec2 startPos )
{
	Item* tempItem = new Item( startPos );
	m_activeItem = tempItem;
	m_items.push_back( tempItem );
}

void Map::CheckCollision()
{
	CheckActorsCollision();
	CheckTilesCollision();
	CheckBulletsCollision();
}

void Map::CheckActorsCollision()
{
	// player and player
	for( int i = 0; i < m_players.size(); i++ ) {
		for( int j = i + 1; j < m_players.size(); j++ ) {
			if( m_players[i]->GetAliveState() == ALIVE && m_players[j]->GetAliveState() == ALIVE ) {
				CheckCollisionBetweenActors( m_players[i], m_players[j] );
			}
		}
	}

	// enemy and enemy
	for( int i = 0; i < m_enemies.size(); i++ ) {
		for( int j = i + 1; j < m_enemies.size(); j++ ) {
			if( m_enemies[i] != nullptr && m_enemies[j] != nullptr )
				CheckCollisionBetweenActors( m_enemies[i], m_enemies[j] );
		}
	}

	// player and enemy
	for( int i = 0; i < m_players.size(); i++ ) {
		for( int j = 0; j < m_enemies.size(); j++ ) {
			if( m_players[i] != nullptr && m_enemies[j] != nullptr && m_players[i]->GetAliveState() == ALIVE ) {
				CheckCollisionBetweenActors( m_players[i], m_enemies[j] );
			}
		}
	}
}

void Map::CheckTilesCollision()
{
	for( int i = 0; i < m_players.size(); i++ ) {
		Player* tempPlayer = m_players[i];
		if( tempPlayer == nullptr ){ continue; }
		CheckCollisionBetweenActorAndTiles( tempPlayer );
	}

	for( int i = 0; i < m_enemies.size(); i++ ) {
		Enemy* tempEnemy = m_enemies[i];
		if( tempEnemy == nullptr ){ continue; }
		CheckCollisionBetweenActorAndTiles( tempEnemy );
	}
}

void Map::CheckBulletsCollision()
{
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		Projectile* tempProj = m_projectiles[i];
		if( tempProj == nullptr ){ continue; }
		CheckCollisionBetweenProjectileAndActors( tempProj );
		CheckCollisionBetweenProjectileAndTiles( tempProj );
	}
}

void Map::CheckCollisionBetweenProjectileAndActors( Projectile* projectile )
{
	switch( projectile->GetType() )
	{
	case ACTOR_ENEMY:
		for( int i = 0; i < m_players.size(); i++ ) {
			Player* tempPlayer = m_players[i];
			if( tempPlayer->GetAliveState() != ALIVE ){ continue; }
			if( IsPointInsideDisc( projectile->GetPosition(), tempPlayer->GetPosition(), tempPlayer->GetPhysicsRadius() ) ) {
				tempPlayer->TakeDamage( projectile->GetDamage() );
				CameraController* playerController = g_theCameraSystem->GetCameraControllerWithPlayer( tempPlayer );
				playerController->AddTrauma( 0.25f );
				projectile->Die();
				break;
			}
		}
	break;
	case ACTOR_PLAYER:
		for( int i = 0; i < m_enemies.size(); i++ ) {
			Enemy* tempEnemy = m_enemies[i];
			if( tempEnemy == nullptr ){ continue; }
			if( IsPointInsideDisc( projectile->GetPosition(), tempEnemy->GetPosition(), tempEnemy->GetPhysicsRadius() ) ) {
				tempEnemy->TakeDamage( projectile->GetDamage() );
				projectile->Die();
				break;
			}
		}
		if( m_boss ) {
			if( IsPointInsideDisc( projectile->GetPosition(), m_boss->GetPosition(), m_boss->GetPhysicsRadius() ) ) {
				m_boss->TakeDamage( projectile->GetDamage() );
				projectile->Die();
				CameraController* playerController = g_theCameraSystem->GetCameraControllerWithPlayer( m_boss );
				playerController->AddTrauma( 0.25f );
				break;
			}
		}
	break;
	case ACTOR_BOSS:
		for( int i = 0; i < m_players.size(); i++ ) {
			Player* tempPlayer = m_players[i];
			if( tempPlayer->GetAliveState() != ALIVE ){ continue; }
			if( IsPointInsideDisc( projectile->GetPosition(), tempPlayer->GetPosition(), tempPlayer->GetPhysicsRadius() ) ) {
				tempPlayer->TakeDamage( projectile->GetDamage() );
				CameraController* playerController = g_theCameraSystem->GetCameraControllerWithPlayer( tempPlayer );
				playerController->AddTrauma( 0.25f );
				projectile->Die();
				break;
			}
		}
	break;
	}
}

void Map::CheckCollisionBetweenProjectileAndTiles( Projectile* projectile )
{
	Vec2 projectilePos = projectile->GetPosition();
	if( IsTileSolid( (IntVec2)projectilePos ) ) {
		projectile->Die();
	}
}

void Map::ManageGarbage()
{
	// manage player
	for( int i = 0; i < m_players.size(); i++ ) {
		Player* tempPlayer = m_players[i];
		if( tempPlayer != nullptr && tempPlayer->GetAliveState() == AliveState::READY_TO_DELETE_PLAYER ) {
			delete tempPlayer;
			m_players.erase( m_players.begin() + i );
		}
	}
	if( m_boss ) {
		if( m_boss->GetAliveState() == AliveState::READY_TO_DELETE_PLAYER ) {
			delete m_boss;
			m_boss = nullptr;
		}
	}

	// manage enemy
	for( int i = 0; i < m_enemies.size(); i++ ) {
		Enemy* tempEnemy = m_enemies[i];
		if( tempEnemy != nullptr && tempEnemy->GetAliveState() == AliveState::WAIT_FOR_DELETE ) {
			delete tempEnemy;
			m_enemies[i] = nullptr;
		}
	}

	// manage projectiles
	for( int i = 0; i < m_projectiles.size(); i++ ) {
		Projectile* tempProjectile = m_projectiles[i];
		if( tempProjectile != nullptr && tempProjectile->GetIsDead() ) {
			delete tempProjectile;
			m_projectiles[i] = nullptr;
		}
	}
}

void Map::CheckCollisionBetweenActors( Actor* actorA, Actor* actorB )
{
	Vec2 posA = actorA->GetPosition();
	Vec2 posB = actorB->GetPosition();
	float radiusA = actorA->GetPhysicsRadius();
	float radiusB = actorB->GetPhysicsRadius();
	if( GetDistanceSquared2D( posA, posB ) < GetQuadraticSum( actorA->GetPhysicsRadius(), actorB->GetPhysicsRadius() ) ) {
		if( actorA->GetDoesPushActor() && !actorA->GetIsPushedByActor() && actorB->GetIsPushedByActor() ) {
			PushDiscOutOfDisc2D( posB, radiusB, posA, radiusA );
		}
		else if( actorB->GetDoesPushActor() && !actorB->GetIsPushedByActor() && actorA->GetIsPushedByActor() ) {
			PushDiscOutOfDisc2D( posA, radiusA, posB, radiusA );
		}
		else if( actorA->GetDoesPushActor() && actorB->GetDoesPushActor() && actorA->GetIsPushedByActor() && actorB->GetIsPushedByActor() ) {
			PushDiscsOutOfEachOther2D( posA, actorA->GetPhysicsRadius(), posB, actorB->GetPhysicsRadius() );
		}
		actorA->SetPosition( posA );
		actorB->SetPosition( posB );
	}
}

void Map::CheckCollisionBetweenActorAndTiles( Actor* actor )
{
	IntVec2 actorrTileCoords = (IntVec2)actor->GetPosition();

	IntVec2 downTileCoords	= actorrTileCoords + IntVec2( 0, -1 );
	IntVec2 upTileCoords	= actorrTileCoords + IntVec2( 0, 1 );
	IntVec2 rightTileCoords	= actorrTileCoords + IntVec2( 1, 0 );
	IntVec2 leftTileCoords	= actorrTileCoords + IntVec2( -1, 0 );

	IntVec2 downLeftTileCoords	= actorrTileCoords + IntVec2( -1, -1 );
	IntVec2 downRightTileCoords	= actorrTileCoords + IntVec2( 1, -1 );
	IntVec2 upLeftTileCoords	= actorrTileCoords + IntVec2( -1, 1 );
	IntVec2 upRightTileCoords	= actorrTileCoords + IntVec2( 1, 1 );

	CheckCollisionBetweenActorAndTile( actor, downTileCoords );
	CheckCollisionBetweenActorAndTile( actor, upTileCoords );
	CheckCollisionBetweenActorAndTile( actor, rightTileCoords );
	CheckCollisionBetweenActorAndTile( actor, leftTileCoords );

	CheckCollisionBetweenActorAndTile( actor, downLeftTileCoords );
	CheckCollisionBetweenActorAndTile( actor, downRightTileCoords );
	CheckCollisionBetweenActorAndTile( actor, upLeftTileCoords );
	CheckCollisionBetweenActorAndTile( actor, upRightTileCoords );
}


void Map::CheckCollisionBetweenActorAndTile( Actor* actor, IntVec2 tileCoords )
{
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_RECOVERABLE( "tileCoords not valid !" );
		return;
	}

	if( !IsTileSolid( tileCoords ) && !IsTileWalkable( tileCoords ) ) { return; }

	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	Tile tempTile = m_tiles[tileIndex];
	AABB2 tileBound = tempTile.GetTileBounds();
	Vec2 playerPos = actor->GetPosition();
	PushDiscOutOfAABB2D( playerPos, actor->GetPhysicsRadius(), tileBound );
	actor->SetPosition( playerPos );
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
	if( m_name == "level2" ) {
		for( int i = 0; i < m_height * m_width; i++ ) {
			IntVec2 tileCoords = GetTileCoordsWithTileIndex( i );
			m_tiles.emplace_back( tileCoords, m_definition->GetEdgeType() );
			m_tileAttributes.push_back( (uint)0 );
		}
	}
	else {
		for( int i = 0; i < m_height * m_width; i++ ) {
			IntVec2 tileCoords = GetTileCoordsWithTileIndex( i );
			m_tiles.emplace_back( tileCoords, m_definition->GetFillType() );
			m_tileAttributes.push_back( (uint)0 );
		}
	}
}

void Map::GenerateEdges()
{
	TileType edgeType = m_definition->GetEdgeType();
	for( int i = 0; i < m_width; i++ ) {
		SetTileTypeWithCoords( IntVec2( i, 0 ), edgeType );
		SetTileTypeWithCoords( IntVec2( i, ( m_height - 1 )), edgeType );
		SetTileSolid( IntVec2( i, 0 ), true );
		SetTileSolid( IntVec2( i, ( m_height - 1 )), true );
	}

	for( int i = 0; i < m_height; i++ ) {
		SetTileTypeWithCoords( IntVec2( 0, i ), edgeType );
		SetTileTypeWithCoords( IntVec2( ( m_width - 1 ), i ), edgeType );
		SetTileSolid( IntVec2( 0, i ), true );
		SetTileSolid( IntVec2( ( m_width - 1 ), i ), true );
	}
}

void Map::RunMapGenSteps()
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

void Map::InitializeMapDifferentTiles()
{

	if( m_name == "level1" )  {
		m_startCoords = GetRandomInsideNotSolidTileCoords();
	}
	else if( m_name == "level2" ) {
		// initialize edge & locked door
		TileType edgeType = m_definition->GetEdgeType();
		TileType lockedType = m_definition->GetLockedType();
		TileType triggerType = m_definition->GetTriggerType();

		int halfHeight = m_height / 2;
		m_doorTiles.push_back( IntVec2( 40, halfHeight ));
		m_doorTiles.push_back( IntVec2( 40, halfHeight - 1 ));
		m_doorTiles.push_back( IntVec2( 80, halfHeight ));
		m_doorTiles.push_back( IntVec2( 80, halfHeight - 1 ));
		for( int i = 0; i < m_height; i++ ) {
			if( i == halfHeight ||  i == halfHeight - 1 ) {
				SetTileTypeWithCoords( IntVec2( 40, i), lockedType );
				SetTileTypeWithCoords( IntVec2( 80, i), lockedType );
				SetTileSolid( IntVec2( 40, i ), true );
				SetTileSolid( IntVec2( 80, i ), true );
			}
			else {
				SetTileTypeWithCoords( IntVec2( 40, i ), edgeType );
				SetTileTypeWithCoords( IntVec2( 80, i ), edgeType );
				SetTileSolid( IntVec2( 40, i ), true );
				SetTileSolid( IntVec2( 80, i ), true );
			}
		}

		// initialize trigger tiles
		m_triggerTiles.push_back( IntVec2( 2, m_height - 3 ) );
		m_triggerTiles.push_back( IntVec2( 78, 3 ));
		for( int i = 0; i < m_triggerTiles.size(); i++ ) {
			SetTileTypeWithCoords( m_triggerTiles[i], triggerType );
			SetTileSolid( m_triggerTiles[i], false );
		}
		m_startCoords = IntVec2( 10, (int)m_height / 2 );
	}


}

void Map::CheckNeedGenerateEnemies()
{
	if( m_isEnemiesGenerated ){ return; }
	if( m_name != "level2" ){ return; }
	Vec2 player1Pos = m_players[0]->GetPosition();
	Vec2 player2Pos = m_players[1]->GetPosition();
	if( player1Pos.x > 40.f && player2Pos.x > 40.f ) {
		g_theEventSystem->SetTimerByFunction( 2.f, this, &Map::GenerateEnemies );
		m_isEnemiesGenerated = true;
	}
}

void Map::CheckNeedGenerateBoss()
{
	if( m_isBossGenerated ){ return; }
	if( m_name != "level2" ){ return; }
	Vec2 player1Pos = m_players[0]->GetPosition();
	Vec2 player2Pos = m_players[1]->GetPosition();
	if( player1Pos.x > 82.f && player2Pos.x > 82.f ) {
		GenerateBoss();
		m_isBossGenerated = true;
	}
}

void Map::CheckPlayerStates()
{
	if( m_name != "level2" ){ return; }
	float togetherDamage	= 15.f;
	float splitDamage		= 10.f;
	float togetherMaxSpeed	= 3.f;
	float splitMaxSpeed		= 5.f;
	Vec2 player1Pos = m_players[0]->GetPosition();
	Vec2 player2Pos = m_players[1]->GetPosition();
	float distanceSq = GetDistanceSquared2D( player1Pos, player2Pos );
	if( distanceSq < 100.f ) {
		m_players[0]->SetIsStronger( true );
		m_players[1]->SetIsStronger( true );
		m_players[0]->SetDamage( togetherDamage );
		m_players[1]->SetDamage( togetherDamage );
		m_players[0]->SetMaxSpeed( togetherMaxSpeed );
		m_players[1]->SetMaxSpeed( togetherMaxSpeed	);
	}
	else {
		m_players[0]->SetIsStronger( false );
		m_players[1]->SetIsStronger( false );
		m_players[0]->SetDamage( splitDamage );
		m_players[1]->SetDamage( splitDamage );
		m_players[0]->SetMaxSpeed( splitMaxSpeed );
		m_players[1]->SetMaxSpeed( splitMaxSpeed );
	}
}

bool Map::GenerateEnemies( EventArgs& args )
{
	UNUSED( args );
	for( int i = 0; i < 3; i++ ) {
		Vec2 enemyPos = GetRandomEnemyPos();
		SpawnNewEnemy( enemyPos );
	}
	return true;
}

void Map::GenerateBoss()
{
	m_boss = Player::SpawnBossWithPos( Vec2( 110.f,15.f ), (int)m_players.size() );	
	m_boss->SetMap( this ); 
	m_boss->SetPhysicsRadius( 4.f );
	g_theCameraSystem->CreateAndPushController( m_boss );
	CameraController* bossController = g_theCameraSystem->GetCameraControllerWithPlayer( m_boss );
	bossController->SetUseCameraFrame( false );
	bossController->SetAllowMerge( false );
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
	case TILE_IS_WALKABLE:	return TILE_IS_WALKABLE_BIT;
	case TILE_IS_DOOR:		return TILE_IS_DOOR_BIT;
	default:
		ERROR_RECOVERABLE( "no attribute!" );
	}
	return TILE_NON_ATTR_BIT;
}
//////////////////////////////////////////////////////////////////////////
