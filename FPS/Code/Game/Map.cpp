#include "Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Physics/RaycastResult.hpp"

extern RenderContext* g_theRenderer;
extern Camera* g_camera;
extern Game* g_theGame;


Map::Map(World* world, const IntVec2& tileDimensions)
	:m_world(world)
	,m_size(tileDimensions)
{
	m_tileNum = m_size.x * m_size.y;
	PopulateTiles();
	SpawnNewEntity(ENTITY_TYPE_PLAYER, ENTITY_FACTION_GOOD, Vec2(1.5 , 1.5)  );
}

Map::~Map()
{
	m_world = nullptr;
	DestroyEntities();
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	const int temIndex = tileCoords.x + (tileCoords.y * m_size.x);
	return temIndex;
}

IntVec2 Map::GetTileCoordsForTileIndex( const int tileIndex ) const
{
	int tileY = tileIndex / m_size.x;
	int tileX = tileIndex - (tileY * m_size.x);
	const IntVec2 temTileCoords = IntVec2(tileX,tileY);
	return temTileCoords;
}

void Map::SetTileType( int tileX, int tileY, TileType tileType )
{
	IntVec2 temVec2 = IntVec2(tileX,tileY);
	int tileIndex = GetTileIndexForTileCoords(temVec2);
	m_tiles[tileIndex].SetTileType(tileType);
}

void Map::PopulateTiles()
{
	//set stone
	for(int tileIndex = 0; tileIndex < m_tileNum; tileIndex++){
		int tileX = tileIndex % m_size.x;
		int tileY = tileIndex / m_size.x;
		m_tiles.push_back(Tile(tileX,tileY));
	}
	for(int tileX = 0; tileX < m_size.x; tileX++){
		SetTileType( tileX,0,TILE_TYPE_STONE);
		SetTileType( tileX,m_size.y-1,TILE_TYPE_STONE);
	}
	for( int tileY=0; tileY < m_size.y; tileY++ ) {
		SetTileType( 0, tileY, TILE_TYPE_STONE );
		SetTileType( m_size.x-1, tileY, TILE_TYPE_STONE );
	}	
	for(int tileY = 2; tileY < 6; tileY++){
		SetTileType( 5,tileY,TILE_TYPE_STONE);
		SetTileType( m_size.x-6,m_size.y-1-tileY,TILE_TYPE_STONE);
	}
	for( int tileX = 2; tileX < 6; tileX++ ) {
		SetTileType( tileX, 5, TILE_TYPE_STONE );
		SetTileType( m_size.x-1-tileX, m_size.y-6, TILE_TYPE_STONE );
	}
	for(int rockIndex = 0; rockIndex < RANDOM_ROCK_NUM; rockIndex++){
		int tileX = g_theGame->m_rng->RollRandomIntInRange(0,m_size.x-1);
		int tileY = g_theGame->m_rng->RollRandomIntInRange(0,m_size.y-1);
		if((tileX < 5 && tileY < 5) || (tileX > m_size.x-5 && tileY > m_size.y-5)){
			rockIndex--;
			continue;
		}
		SetTileType(tileX,tileY,TILE_TYPE_STONE);
	}
}

void Map::RenderEntities() const
{

	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ ) {
		for( int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++){
			m_entities[entityTypeIndex][entityIndex]->Render();
		}
	}
}

void Map::UpdateEntities( float deltaTime )
{
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ ) {
		for( int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++ ) {
			m_entities[entityTypeIndex][entityIndex]->Update(deltaTime);
		}
	}
	
}

void Map::DestroyEntities()
{
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ ) {
		for( int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++ ) {
			delete m_entities[entityTypeIndex][entityIndex];
			m_entities[entityTypeIndex][entityIndex] = nullptr;
		}
	}
}

void Map::SpawnNewEntity( EntityType entityType, EntityFaction entityFaction, const Vec2& spawnPosition )
{
	UNUSED( spawnPosition );
	UNUSED( entityFaction );
	Entity* entity = nullptr;
	switch( entityType )
	{
	case ENTITY_TYPE_NULL:
		break;
	case ENTITY_TYPE_PLAYER:			 entity = new Player();
		break;
	
		/*
	case ENTITY_TYPE_NPC_TURRET:
		break;
	case ENTITY_TYPE_NPC_TANK:
		break;
	case ENTITY_TYPE_BOULDER:
		break;
	case ENTITY_TYPE_BULLET:
		break;*/
	default:
		break;
	}
	AddEntityToMap(entity);
}



bool Map::IsTileSolid( const IntVec2& tileCoords ) const
{
	UNUSED( tileCoords );
	//int tileIndex = GetTileIndexForTileCoords(tileCoords);
	//TileType tileType = m_tiles[tileIndex].m_type;
	return true;//TileDefinition::s_definitions[tileType].GetTileIsSolid();
}


RaycastResult Map::Raycast( Vec2 start, Vec2 fwdDir, float maxDist )
{
	//TODO 
	//float duration = maxDist / 100 ;
	float currentDist = 0;
	while( currentDist < maxDist){
		Vec2 currentPoint = start + (fwdDir * currentDist);
		// Check tile first
		IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos( currentPoint); 
	}
	RaycastResult tem;
	return tem;
}

void Map::HasLineOfSight()
{

}

void Map::UpdateCamera(  )
{
	if(!m_entities[ENTITY_TYPE_PLAYER][0]){
		return;
	}
	Vec2 playerPos = m_entities[ENTITY_TYPE_PLAYER][0]->m_position;
	AABB2 cameraAABB2 = g_camera->GetCameraAsBox();
	g_camera->SetPosition( Vec3(playerPos ) );
	cameraAABB2.filledWithinAABB2(AABB2(Vec2(0,0),Vec2(MAP_SIZE_X,MAP_SIZE_Y)));
}

void Map::CheckCollision()
{
	Entity* player = m_entities[ENTITY_TYPE_PLAYER][0];//dangerous. should be zero in player list

	//check collide with 8 collides.
	IntVec2 playerTileCoords	=	IntVec2( player->m_position );
	IntVec2 downTile			=	playerTileCoords+IntVec2( 0, -1 );
	IntVec2 upTile				=	playerTileCoords+IntVec2( 0, 1 );
	IntVec2 leftTile			=	playerTileCoords+IntVec2( -1, 0 );
	IntVec2 rightTile			=	playerTileCoords+IntVec2( 1, 0 );
	IntVec2 leftDownTile		=	playerTileCoords+IntVec2( -1, -1 );
	IntVec2 rightUpTile			=	playerTileCoords+IntVec2( 1, 1 );
	IntVec2 leftUpTile			=	playerTileCoords+IntVec2( -1, 1 );
	IntVec2 rightDownTile		=	playerTileCoords+IntVec2( 1, -1 );

	CheckPlayerTileCollisionWithTileCoords( player, downTile );
	CheckPlayerTileCollisionWithTileCoords( player, upTile );
	CheckPlayerTileCollisionWithTileCoords( player, leftTile );
	CheckPlayerTileCollisionWithTileCoords( player, rightTile );

	CheckPlayerTileCollisionWithTileCoords( player, leftDownTile );
	CheckPlayerTileCollisionWithTileCoords( player, rightUpTile );
	CheckPlayerTileCollisionWithTileCoords( player, leftUpTile );
	CheckPlayerTileCollisionWithTileCoords( player, rightDownTile );
}


void Map::CheckPlayerTileCollisionWithTileCoords( Entity* player, IntVec2 tileCoords )
{
	if(!IsTileCoordsValid(tileCoords)){
		return;
	}

	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	Tile temTile = m_tiles[tileIndex];
	if( temTile.m_type == TILE_TYPE_GRASS ) { return; }
	else {
		AABB2 temAABB2 = temTile.GetBounds();
		PushDiscOutOfAABB2D( player->m_position, player->m_physicsRadius, temAABB2 );
	}
}

void Map::AddEntityToMap( Entity* entity )
{
	if( entity == nullptr){ return ;}
	m_entities[entity->m_type].push_back(entity);
}

bool Map::IsTileCoordsValid( const IntVec2& tileCoords ) const
{
	if(tileCoords.x >= 0 && tileCoords.y >= 0 && tileCoords.x < m_size.x && tileCoords.y < m_size.y){
		return true;
	}
	else{
		return false;
	}
}

bool Map::IsTileIndexValid( int tileIndex ) const
{
	if(tileIndex >= 0 && tileIndex <= m_tileNum){return true;}
	else{return false;}
}

void Map::RenderTiles() const
{
	g_theRenderer->BindTexture(nullptr);
	for(int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++){
		m_tiles[tileIndex].Render();
	}
}

void Map::Update(float deltaTime)
{
	UpdateEntities(deltaTime);
	if(!g_theGame->m_debugCamera){
		UpdateCamera();
	}
	if(!g_theGame->m_noClip ) {
		CheckCollision();
	}

}

void Map::Render() const
{	g_theRenderer->BeginCamera(*g_camera);
	RenderTiles();
	RenderEntities();
	g_theRenderer->EndCamera(*g_camera);

}

