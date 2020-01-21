#include "Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/World.hpp"
#include "Game/Player.hpp"
#include "Game/Explosion.hpp"
#include "Game/NpcTurret.hpp"
#include "Game/NpcTank.hpp"
#include "Game/Bullet.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/BitmapFont.hpp"	
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

extern RenderContext* g_theRenderer;
extern Camera* g_camera;
extern Game* g_theGame;
extern InputSystem* g_theInputSystem;
extern BitmapFont* g_squirrelFont;

Map::Map( World* world, const IntVec2& tileDimensions, std::vector<TileType> wormTypes, int* numWorm, int* wormLength, TileType edgeType, TileType defaultType )
	:m_world(world)
	,m_size(tileDimensions)
	,m_wormTypes(wormTypes)
	,m_numWorms(numWorm)
	,m_wormLength(wormLength)
	,m_defaultType(defaultType)
	,m_edgeType(edgeType)
{
	m_tileNum = m_size.x * m_size.y;
	m_exitPos = m_size - IntVec2(2,2);
	CreateMap();
	CreateEnemy();
	SpawnNewEntity(ENTITY_TYPE_PLAYER, ENTITY_FACTION_GOOD, Vec2(m_startPos)  );
	g_theGame->m_isPlayerDead = false;
	
}

Map::~Map()
{
	m_world = nullptr;
	DestroyEntities();
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	int temIndex = tileCoords.x + (tileCoords.y * m_size.x);
	return temIndex;
}

IntVec2 Map::GetTileCoordsForTileIndex( const int tileIndex ) const
{
	int tileY = tileIndex / m_size.x;
	int tileX = tileIndex - (tileY * m_size.x);
	IntVec2 temTileCoords = IntVec2(tileX,tileY);
	return temTileCoords;
}

void Map::SetTileType( int tileX, int tileY, TileType tileType )
{
	IntVec2 temVec2 = IntVec2(tileX,tileY);
	int tileIndex = GetTileIndexForTileCoords(temVec2);
	m_tiles[tileIndex].SetTileType(tileType);
}

void Map::SetTileType( IntVec2 tilePos, TileType tileType )
{
	int tileIndex = GetTileIndexForTileCoords(tilePos);
	m_tiles[tileIndex].SetTileType(tileType);
}

void Map::SetWormTileType( IntVec2 tilePos, TileType tileType )
{
	int tileIndex = GetTileIndexForTileCoords( tilePos );
	m_tiles[tileIndex].SetTileType( tileType );
	m_wormTilesRecords[tileIndex] = true;

}

void Map::SetTileReachable(IntVec2 tilePos)
{
	int tileIndex = GetTileIndexForTileCoords( tilePos );
	m_reachableTilesRecords[tileIndex] = true;
}

void Map::SetTileEdge( IntVec2 tilePos )
{
	int tileIndex = GetTileIndexForTileCoords( tilePos );
	m_edgeTilesRecords[tileIndex] = true;
}

void Map::SetTileEdge( int tileX, int tileY )
{
	IntVec2 tilePos	= IntVec2(tileX, tileY);
	int tileIndex = GetTileIndexForTileCoords( tilePos );
	m_edgeTilesRecords[tileIndex] = true;
}

IntVec2 Map::GetNextWormPos( IntVec2 currentPos )
{
	IntVec2 nextPos;
	do 
	{
		int temDirection = m_rng.RollRandomIntInRange( 0, 3 );
		switch( temDirection )
		{
		case 0:
			nextPos = currentPos + IntVec2(1,0);
			break;
		case 1:
			nextPos = currentPos + IntVec2(-1,0);
			break;
		case 2:
			nextPos = currentPos + IntVec2(0,1);
			break;
		case 3:
			nextPos = currentPos + IntVec2(0,-1);
			break;
		}
	} while (!IsTileCoordsValid(nextPos));
	return nextPos;
	
}

bool Map::IsValidWormPos( const IntVec2 tileCoords ) const
{
	if(!IsTileCoordsValid(tileCoords)){return false;}
	if(IsTileStart(tileCoords)){return false;}
	if(IsTileExit(tileCoords)){return false;}
	if(IsTileEdge(tileCoords)){return false;}
	if(IsTileWormed(tileCoords)){
		return false;
	}
	else{
		return true;
	}


}

bool Map::isValidCurrentWormPos( const IntVec2 tileCoords ) const
{
	if(!IsTileCoordsValid(tileCoords)){return false;}
	IntVec2 leftTileCoords = tileCoords + IntVec2(-1,0);
	IntVec2 rightTileCoords = tileCoords + IntVec2(1,0);
	IntVec2 upTileCoords = tileCoords + IntVec2(0,1);
	IntVec2 downTileCoords = tileCoords + IntVec2(0,-1);

	if(IsValidWormPos(leftTileCoords)){return true;}
	if(IsValidWormPos(rightTileCoords)){return true;}
	if(IsValidWormPos(upTileCoords)){return true;}
	if(IsValidWormPos(downTileCoords)){return true;}

	return false;
	

}


Vec2 Map::GetNonSolidPos()
{
	IntVec2 tileCoords =  GenerateRandomValidTilePosInMap();
	Vec2 worldCoords = Vec2(tileCoords);
	while(isPointInSolid(worldCoords)){
		tileCoords = GenerateRandomValidTilePosInMap();
		worldCoords = Vec2(tileCoords);
	}
	return worldCoords;
}

void Map::PopulateTiles()
{
	m_startType = m_defaultType;
	m_exitType = m_defaultType;
	
	InitializeTiles();
	GenerateStartTiles();
	GenerateExitTiles();
	GenerateEdgeTiles();
	GenerateWormTiles();

	m_isValidMap = IsExitPosReachable();

	UpdateSolidTiles();
	UpdateBrickTiles();
	
}

void Map::pushTileVertices()
{
	m_tileVertices.clear();
	for( int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ ) {
		Tile temTile = m_tiles[tileIndex];
		AABB2 temAABB = m_tiles[tileIndex].GetBounds();
		Rgba8 tintColor = Rgba8( 255, 255, 255 );
		TileDefinition temDef = TileDefinition::s_definitions[temTile.m_type];
		AddVertsForAABB2D( m_tileVertices, temAABB, tintColor, temDef.m_uvAtMins, temDef.m_uvAtMaxs );
	}
}

void Map::GenerateWormTiles()
{
	for(int wormTileTypeIndex = 0; wormTileTypeIndex < m_wormTypes.size(); wormTileTypeIndex ++){
		for(int wormIndex = 0; wormIndex < m_numWorms[wormTileTypeIndex]; wormIndex++){
			TileType temType = m_wormTypes[wormTileTypeIndex];
			int temWormLength = m_wormLength[wormTileTypeIndex];
			IntVec2 wormStartPos = GenerateRandomValidTilePosInMap();
			if(!IsValidWormPos(wormStartPos)){
				continue;
			}
			SetWormTileType(wormStartPos, temType);
			IntVec2 currentWormPos = wormStartPos;
			IntVec2 nextWormPos;
			int currentLength = 1;
			
			while(currentLength < temWormLength){ 
				if(!isValidCurrentWormPos(currentWormPos)){break;}

				nextWormPos = GetNextWormPos(currentWormPos);
				if(IsValidWormPos(nextWormPos)){
					currentWormPos = nextWormPos;
					SetWormTileType(currentWormPos,temType);
					currentLength++;
				}
			}
			
		}
	}
}

void Map::GenerateStartTiles()
{
	//should using  m_startTilesRecords.
	for( int tileX = 1; tileX < 6; tileX++ ) {
		for( int tileY = 1; tileY < 6; tileY++ ) {
			SetTileType( tileX, tileY, m_startType );
		}
	}
}

void Map::GenerateExitTiles()
{
	//should use m_exitTilesRecords;
	for( int tileX = 1; tileX < 6; tileX++ ) {
		for( int tileY = 1; tileY < 6; tileY++ ) {
			SetTileType( m_size.x-1-tileX, m_size.y-1-tileY, m_exitType );
		}
	}
}

void Map::GenerateEdgeTiles()
{
	for( int tileX = 0; tileX < m_size.x; tileX++ ) {
		SetTileType( tileX, 0, m_edgeType );
		SetTileType( tileX, m_size.y-1, m_edgeType );
		SetTileEdge(tileX, 0);
		SetTileEdge(tileX,  m_size.y-1);
	}
	for( int tileY=0; tileY < m_size.y; tileY++ ) {
		SetTileType( 0, tileY, m_edgeType );
		SetTileType( m_size.x-1, tileY, m_edgeType );
		SetTileEdge( 0, tileY );
		SetTileEdge( m_size.x-1, tileY );
	}

	for( int tileY = 2; tileY < 7; tileY++ ) {
		SetTileType( 6, tileY, m_edgeType );
		SetTileType( m_size.x-7, m_size.y-1-tileY, m_edgeType );
		SetTileEdge( 6, tileY );
		SetTileEdge( m_size.x-7, m_size.y-1-tileY);
	}
	for( int tileX = 2; tileX < 7; tileX++ ) {
		SetTileType( tileX, 6, m_edgeType );
		SetTileType( m_size.x-1-tileX, m_size.y-7, m_edgeType );
		SetTileEdge( tileX, 6 );
		SetTileEdge(m_size.x-1-tileX, m_size.y-7 );
	}
}

void Map::UpdateSolidTiles()
{
	for( int tileIndex = 0; tileIndex < m_tileNum; tileIndex++ ) {
		int tileX = tileIndex % m_size.x;
		int tileY = tileIndex / m_size.x;
		IntVec2 temTileCoords = IntVec2(tileX, tileY);
		int temTileIndex = GetTileIndexForTileCoords(temTileCoords);
		if(IsTileSolid(temTileCoords)){
			m_solidTilesRecords[temTileIndex] = true;
		}
		else{
			if(!IsTileReachable(temTileCoords)){
				SetTileType(temTileCoords, m_edgeType);
				m_solidTilesRecords[temTileIndex] = true;
			}
		}
	}

}



void Map::SetBrickTileToDeault( Vec2 point )
{
	IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos(point); 
	if(IsTileOfType(currentPointInTileCoords, TileType::TILE_TYPE_BRICK)){
		SetTileType(currentPointInTileCoords,m_defaultType);
	}
}

void Map::UpdateBrickTiles()
{
	for( int tileIndex = 0; tileIndex < m_tileNum; tileIndex++ ) {
		int tileX = tileIndex % m_size.x;
		int tileY = tileIndex / m_size.x;
		IntVec2 temTileCoords = IntVec2( tileX, tileY );
		int temTileIndex = GetTileIndexForTileCoords( temTileCoords );
		if( IsTileOfType( temTileCoords, TileType::TILE_TYPE_BRICK ) ) {
			m_brickDestroiedRecords[temTileIndex] = false;
		}
		
	}
}

void Map::UpdateBrickTilesToDefault()
{

}

bool Map::IsExitPosReachable()
{
	UpdateReachableRecords();
	int exitTileIndex = GetTileIndexForTileCoords(m_exitPos);
	if(m_reachableTilesRecords[exitTileIndex] == true){
		return true;
	}
	else{
		return false;
	}
}

void Map::UpdateReachableRecords()
{
	FloodFillTiles(m_startPos.x, m_startPos.y);
}




void Map::FloodFillTiles( int tileX, int tileY )
{
	IntVec2 temTileCoords = IntVec2(tileX, tileY);

	if( !IsTileCoordsValid( temTileCoords ) ) { return; }
	if( IsTileSolid( temTileCoords ) ) { return; }
	if( IsFloodFilledOnce( temTileCoords ) ) { return; }
	SetTileReachable(temTileCoords);

	FloodFillTiles(tileX + 1, tileY);
	FloodFillTiles(tileX - 1, tileY);
	FloodFillTiles(tileX, tileY + 1);
	FloodFillTiles(tileX, tileY - 1);

}

bool Map::IsFloodFilledOnce( IntVec2 tileCoords )
{
	int tileIndex = GetTileIndexForTileCoords(tileCoords);
	if(m_reachableTilesRecords[tileIndex] == true){
		return true;
	}
	else{
		return false;
	}
}

IntVec2 Map::GenerateRandomValidTilePosInMap()
{
	int temX = (int)m_rng.RollRandomFloatInRange(1,(float)m_size.x-1);
	int temY = (int)m_rng.RollRandomFloatInRange(1,(float)m_size.y-1);
	return IntVec2(temX, temY);
}

void Map::CheckRespawnPlayerInput()
{
	if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_N ) ) {
		CreateNewPlayer();
	}
}

void Map::CreateMap()
{
	TileDefinition::InitializeDefinitions();
	while( !m_isValidMap ) {
		PopulateTiles();
	}
	pushTileVertices();
}

void Map::CreateEnemy()
{
	Vec2 randomPos;
	if( m_world->m_mapCreateIndex == 0 ) {
		randomPos = GetNonSolidPos();
		SpawnNewEntity(ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos);
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
	}
	else if( m_world->m_mapCreateIndex == 1 ){
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
	}
	else if( m_world->m_mapCreateIndex == 2 ) {
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );	
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
	}
	else{
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TURRET, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
		randomPos = GetNonSolidPos();
		SpawnNewEntity( ENTITY_TYPE_NPC_TANK, ENTITY_FACTION_EVIL, randomPos );
	}

}

void Map::CreateNewPlayer()
{
	if(m_entities[ENTITY_TYPE_PLAYER][0] != nullptr){ return;}
	SpawnNewEntity(ENTITY_TYPE_PLAYER, ENTITY_FACTION_GOOD, m_playerDeadPos  );
	if(m_world->m_playerLife > 0){
		m_world->m_playerLife--;
	}
	m_deadSceneState = DEAD_SCENE_FADE_OUT;
	g_theGame->m_isPlayerDead = false;
}

void Map::CheckIfPlayerExit()
{
	Player* temPlayer = (Player*)m_entities[ENTITY_TYPE_PLAYER][0];
	if( temPlayer == nullptr ) { return; }
	Vec2 pos = temPlayer->m_position;
	IntVec2 posInTileCoords = IntVec2(pos);
	if(m_exitPos == posInTileCoords){
		m_playerExit = true;
	}
	else{
		m_playerExit =false;
	}

}

void Map::CheckIfPlayerInLava()
{
	Player* temPlayer = (Player*)m_entities[ENTITY_TYPE_PLAYER][0];
	if( temPlayer == nullptr ) { return; }
	Vec2 pos = temPlayer->m_position;
	TileType tileType = GetTileTypeInWorld( pos );
	if( tileType == TILE_TYPE_LAVA ) {
		m_isPlayerInLava = true;
	}
	else {
		m_isPlayerInLava = false;
	}
}

void Map::CheckIfPlayerInMud()
{
	Player* temPlayer = (Player*)m_entities[ENTITY_TYPE_PLAYER][0];
	if( temPlayer == nullptr ) { return; }
	Vec2 pos = temPlayer->m_position;
	TileType tileType = GetTileTypeInWorld(pos);
	if(tileType == TILE_TYPE_MUD){
		m_isPlayerInMud = true;
	}
	else{
		m_isPlayerInMud = false;
	}
}



void Map::RenderEntities() const
{

	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ ) {
		if(entityTypeIndex == ENTITY_TYPE_EXPLOSION){
			g_theRenderer->SetBlendMode( BlendMode::ADDITIVE );
		}
		for( int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++){
			Entity* temEntity = m_entities[entityTypeIndex][entityIndex];
			if(temEntity == nullptr){continue;}
			temEntity->Render();
		}
		if( entityTypeIndex == ENTITY_TYPE_EXPLOSION ) {
			g_theRenderer->SetBlendMode( BlendMode::ALPHA );
		}
	}
}





void Map::InitializeTiles()
{
	// initialize the tile vectors
	m_tiles.clear();
	m_wormTilesRecords.clear();
	m_reachableTilesRecords.clear();
	m_edgeTilesRecords.clear();
	m_solidTilesRecords.clear();
	m_brickDestroiedRecords.clear();
	m_exitTilesRecords.clear();
	m_startTilesRecords.clear();

	for( int tileIndex = 0; tileIndex < m_tileNum; tileIndex++ ) {
		int tileX = tileIndex % m_size.x;
		int tileY = tileIndex / m_size.x;
		m_tiles.push_back( Tile( tileX, tileY, m_defaultType ) );
		m_brickDestroiedRecords.push_back( true );
		m_edgeTilesRecords.push_back( false );
		m_wormTilesRecords.push_back( false );
		m_reachableTilesRecords.push_back( false );
		m_solidTilesRecords.push_back( false );
		m_startTilesRecords.push_back( false );
		m_exitTilesRecords.push_back( false );
	}
}

void Map::RenderPlayerDeadScene() const
{
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawAABB2D(m_deadScene,m_deadSceneColor);
	if(m_deadSceneState == DEAD_SCENE_WAIT_FOR_INPUT){
		g_theRenderer->BindTexture(g_squirrelFont->GetTexture());
		g_theRenderer->DrawVertexVector(m_deadMessage);
	}
}



void Map::UpdateEntities( float deltaTime )
{
	CheckEntitiesVisibility();
	CheckEntitiesDeath();
	for( int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES; entityTypeIndex++ ) {
		for( int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++ ) {
			Entity* temEntity = m_entities[entityTypeIndex][entityIndex];
			if( temEntity == nullptr ) { continue; }
			temEntity->Update(deltaTime);
		}
	}
	
}


void Map::UpdatePlayerDeadScene( float deltaSeconds )
{
	switch( m_deadSceneState )
	{
	case DEAD_SCENE_FADE_IN:{
		m_deadSceneFadeInTime += deltaSeconds;
		m_deadScene = AABB2( g_camera->m_AABB2 );
		float temColorAlpha = RangeMapFloat( 0, 2, 0, 100, m_deadSceneFadeInTime );
		temColorAlpha = ClampFloat( 0, 100, temColorAlpha );
		int colorAlpha = RoundDownToInt( temColorAlpha );
		m_deadSceneColor = Rgba8( 0, 0, 0, (unsigned char)colorAlpha );
		if( m_deadSceneFadeInTime >= 2 ) {
			m_deadSceneState = DEAD_SCENE_WAIT_FOR_INPUT;
		}
		break;
	}
	case DEAD_SCENE_WAIT_FOR_INPUT:{
		m_deadMessage.clear();
		g_squirrelFont->AddVertsForText2D( m_deadMessage, Vec2( 4, 5 ), 0.5, "You Died! " );
		g_squirrelFont->AddVertsForText2D( m_deadMessage, Vec2( 4, 4.5 ), 0.5, "Press 'N' to Resume " );
		CheckRespawnPlayerInput();
		break;
	}
	case DEAD_SCENE_FADE_OUT:{
		m_deadSceneFadeOutTime += deltaSeconds;
		m_deadScene = AABB2( g_camera->m_AABB2 );
		float temColorAlpha = RangeMapFloat( 0, 0.5, 100, 0, m_deadSceneFadeOutTime );
		temColorAlpha = ClampFloat( 0, 100, temColorAlpha );
		int colorAlpha = RoundDownToInt( temColorAlpha );
		m_deadSceneColor = Rgba8( 0, 0, 0, (unsigned char)colorAlpha );
		if( m_deadSceneFadeOutTime >= 0.5 ) {
			m_deadSceneState = DEAD_SCENE_NULL;
		}
		break;
	}
	}
	
	
	
	//g_theRenderer->DrawAABB2D(m_deadScene, deadSce)
}
void Map::SpawnNewExplosion( const Vec2& spawnPosition, float radiu, float duration, bool isExplosionInBrick )
{
	Entity* temEntity = SpawnNewEntity(ENTITY_TYPE_EXPLOSION, ENTITY_FACTION_NEUTRAL, spawnPosition);
	Explosion* temExplosion = (Explosion*) temEntity;
	temExplosion->m_radiu = radiu;
	temExplosion->m_duration = duration;
	temExplosion->m_isInBrick = isExplosionInBrick;
	temExplosion->CreateSpriteAnimation();
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


void Map::DeleteEntity( Entity* entity )
{
	if(entity == nullptr){return; }
	delete entity;
	/*

	EntityList::iterator it = std::find( m_entities[entity->m_type].begin(), m_entities[entity->m_type].end(), entity );
	if(it != m_entities[entity->m_type].end()){
		int index = (int)std::distance(m_entities[entity->m_type].begin(), it);
		m_entities[entity->m_type][index] = nullptr;
	}*/
}

Entity* Map::SpawnNewEntity( EntityType entityType, EntityFaction entityFaction, const Vec2& spawnPosition )
{
	Entity* entity = nullptr;
	switch( entityType )
	{
	case ENTITY_TYPE_NULL:
		break;
	case ENTITY_TYPE_PLAYER:			entity = new Player(this, spawnPosition );
		break;
	case ENTITY_TYPE_NPC_TURRET:		entity = new NpcTurret(this, spawnPosition, entityType, entityFaction);
		break;
	case ENTITY_TYPE_NPC_TANK:			entity = new NpcTank(this, spawnPosition);
		break;
	case ENTITY_TYPE_BOULDER:
		break;
	case ENTITY_TYPE_BULLET:			entity = new Bullet(this, spawnPosition, entityType, entityFaction);
		break;
	case ENTITY_TYPE_EXPLOSION:			entity = new Explosion(this, spawnPosition );
		break;
	default:
		break;
	}
	AddEntityToMap(entity);
	return entity;
}





void Map::SpawnNewBullet( EntityFaction entityFaction, const Vec2& spawnPosition, const Vec2& spawnDir )
{
	Entity* temEntity = SpawnNewEntity(ENTITY_TYPE_BULLET, entityFaction, spawnPosition);
	Bullet* temBullet = (Bullet*)temEntity;
	temBullet->m_fwdDir = spawnDir;
}

void Map::CheckEntitiesVisibility()
{
	Player* temPlayer = (Player*) m_entities[ENTITY_TYPE_PLAYER][0];
	if(temPlayer == nullptr){return ;}
	for(int entityTypeIndex = 1; entityTypeIndex < ENTITY_TYPE_BOULDER; entityTypeIndex++ ){
		for(int entityIndex =0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++){
			if(entityTypeIndex == ENTITY_TYPE_NPC_TURRET){
				NpcTurret* temTurret = (NpcTurret*)m_entities[entityTypeIndex][entityIndex];
				if(temTurret == nullptr){continue;}
				CheckVisibilityBetweenPlayerTurret(temPlayer, temTurret);
			}
			if( entityTypeIndex == ENTITY_TYPE_NPC_TANK ) {
				NpcTank* temTank = (NpcTank*)m_entities[entityTypeIndex][entityIndex];
				if(temTank == nullptr){continue;;}
				CheckVisibilityBetweenPlayerTank( temPlayer, temTank );
			}
		}
	}
}

void Map::CheckVisibilityBetweenPlayerTurret( Player* player, NpcTurret* turret )
{
	
	if(!player->IsAlive()){
		turret->m_isPlayerVisiable = false;
		return;
	}
	float maxDist = turret->m_visionDist;
	bool isVisable = HasLineOfSight( turret, player, maxDist );
	if( !isVisable  ) {
		turret->m_isPlayerVisiable = false;
	}
	else {
		turret->m_isPlayerVisiable = true;
		turret->m_targetPlayerPos = player->m_position;
	}
}

void Map::CheckVisibilityBetweenPlayerTank( Player* player, NpcTank* tank )
{
	if( !player->IsAlive() ) {
		tank->m_isPlayerVisiable = false;
		return;
	}
	float maxDist = tank->m_visionDist;
	bool isVisable = HasLineOfSight( tank, player, maxDist );
	if( !isVisable ) {
		tank->m_isPlayerVisiable = false;
	}
	else {
		tank->m_isPlayerVisiable = true;
		tank->m_targetPlayerPos = player->m_position;
	}
}





bool Map::IsTileOfType( const IntVec2& tileCoords, TileType tileType ) const
{
	if( !IsTileCoordsValid( tileCoords ) ) { return false; }//CODE REVIEW:: Is it better inside GetTileIndexForTileCoords()
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	TileType myTileType = m_tiles[tileIndex].m_type;
	if( tileType == myTileType ) {
		return true;
	}
	else {
		return false;
	}
}

bool Map::IsTileBrick( const IntVec2& tileCoords ) const
{
	bool isBrick = IsTileOfType(tileCoords, TILE_TYPE_BRICK); 
	return isBrick;
}

bool Map::IsTileWater( const IntVec2& tileCoords ) const
{
	bool isWater = IsTileOfType(tileCoords, TileType::TILE_TYPE_WATER);
	return isWater;
}

bool Map::IsTileStart( const IntVec2& tileCoords ) const
{
	
	if((tileCoords.x >= 1 && tileCoords.x < 6) && (tileCoords.y >=1 && tileCoords.y <6)){
		return true;
	}
	else{
		return false;
	}
}

bool Map::IsTileExit( const IntVec2& tileCoords ) const
{


	if( (tileCoords.x >= m_size.x -6 && tileCoords.x < m_size.x -1) && (tileCoords.y >= m_size.y -6 && tileCoords.y < m_size.y -1) ) {
		return true;
	}
	else {
		return false;
	}
}

bool Map::IsTileSolid( const IntVec2& tileCoords ) const
{
	if(!IsTileCoordsValid(tileCoords)){return false;}
	int tileIndex = GetTileIndexForTileCoords(tileCoords);
	TileType tileType = m_tiles[tileIndex].m_type;
	return TileDefinition::s_definitions[tileType].GetTileIsSolid();
}

bool Map::IsTileEdge( const IntVec2& tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ) { return false; }
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	return m_edgeTilesRecords[tileIndex];
}

bool Map::IsTileWormed( const IntVec2& tileCoords ) const
{
	if(!IsTileCoordsValid(tileCoords)){return false;}
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	return m_wormTilesRecords[tileIndex];
}

bool Map::IsTileReachable( const IntVec2& tileCoords ) const
{
	if( !IsTileCoordsValid( tileCoords ) ) { return false; }
	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	return m_reachableTilesRecords[tileIndex];
}

bool Map::isPointInSolid( const Vec2& point )
{
	IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos(point); 
	if(!IsTileCoordsValid(currentPointInTileCoords)){ return false;}
	bool isSolid = IsTileSolid(currentPointInTileCoords);
	return isSolid;
}

bool Map::IsPointInType( const Vec2& point, TileType tiletype )
{
	IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos( point );
	bool isType = IsTileOfType( currentPointInTileCoords, tiletype );
	return isType;
}

bool Map::IsPointInBrick( const Vec2& point )
{
	bool isBrick = IsPointInType(point, TILE_TYPE_BRICK);
	return isBrick;
}

bool Map::IsPointInWater( const Vec2& point )
{
	bool isWater = IsPointInType(point,TileType::TILE_TYPE_WATER);
	return isWater;
}

bool Map::IsPointInEntity( const Vec2& point, Entity* entity )
{	
	if(entity == nullptr){return false;}
	Vec2 center = entity->m_position;
	float radius = entity->m_physicsRadius;
	bool result = IsPointInDisc(point, center, radius);
	return result;
}

TileType Map::GetTileTypeInWorld( const Vec2& worldCoords ) const
{
	IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos( worldCoords );
	TileType tileType = GetTileType( currentPointInTileCoords );
	return tileType;
}

TileType Map::GetTileType( const IntVec2& tileCoords ) const
{

	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	TileType tileType = m_tiles[tileIndex].m_type;
	return tileType;
}

RaycastResult Map::Raycast( Vec2 start, Vec2 fwdDir, float maxDist )
{
	
	bool isSolid = false;
	float duration = maxDist / 100 ;
	float currentDist = 0;
	Vec2 currentPoint = Vec2::ZERO;
	TileType tileType = TILE_TYPE_NULL;
	
	// Normalize fwdDir
	if(fwdDir.GetLength() != 1){
		fwdDir.Normalize();
	}

	while( currentDist <= maxDist){
		currentPoint = start + (fwdDir * currentDist);
		// Check tile first
		tileType = GetTileTypeInWorld(currentPoint);
		isSolid = isPointInSolid(currentPoint);
		if(isSolid){
			break;
		}
		currentDist += duration;
	}
	RaycastResult temResult;
	if(isSolid){
		temResult = RaycastResult(true, currentDist, tileType , currentPoint ,start  );
		
	}
	else{
		temResult = RaycastResult(false, currentDist, tileType , currentPoint,start  );	
	}
	
	m_raycastPoints.push_back( temResult.m_start );
	m_raycastPoints.push_back(temResult.m_impactPos);
	return temResult;
}



bool Map::HasLineOfSight( Entity* entitySee, Entity* entitySeen, float maxDist )
{
	Vec2 disp =  entitySeen->m_position - entitySee->m_position ;
	float dist = disp.GetLength() - entitySeen->m_physicsRadius;
	if(dist > maxDist ){ return false ;}

	Vec2 fwdDir = disp.GetNormalized();
	RaycastResult result = Raycast( entitySee->m_position, fwdDir, dist);
	//m_raycastResult.push_back(result);
	if(result.m_didImpact){
		return false;
	}
	else{
		return true;
	}
}





void Map::InitializeDeadScene()
{
	m_deadSceneState = DEAD_SCENE_FADE_IN;
	m_deadSceneFadeInTime = 0;
	m_deadSceneFadeOutTime = 0;
}

void Map::UpdateCamera(  )
{
	if(!m_entities[ENTITY_TYPE_PLAYER][0]){
		return;
	}
	Vec2 playerPos = m_entities[ENTITY_TYPE_PLAYER][0]->m_position;
	AABB2& cameraAABB2 = g_camera->m_AABB2;
	g_camera->SetPosition(playerPos);
	cameraAABB2.filledWithinAABB2(AABB2(Vec2(0,0),Vec2(MAP_SIZE_X,MAP_SIZE_Y)));
}





void Map::CheckEntitiesDeath()
{
	for(int entityTypeIndex = 0; entityTypeIndex < NUM_ENTITY_TYPES ; entityTypeIndex++){
		for(int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++ ){
			Entity* temEntity = m_entities[entityTypeIndex][entityIndex];
			if(temEntity == nullptr){continue;}
			if(!temEntity->IsAlive()){
				DeleteEntity(temEntity);
				m_entities[entityTypeIndex][entityIndex] = nullptr;
			}
		}
	}
}

void Map::CheckCollision()
{
	//Check entities collision;
	CheckEntitiesCollision();
	//Check Tile Collision;
	CheckTileCollision();
	//Check bullet collision;
	CheckBulletCollision();
}


void Map::CheckTileCollision()
{
	if( !g_theGame->m_noClip ) {
		CheckPlayerCollision();
	}
	CheckEnemyTankCollision();
	CheckTurretCollision();
}

void Map::CheckEntitiesCollision()
{
	for(int entityTypeIndex1 = ENTITY_TYPE_PLAYER; entityTypeIndex1 < ENTITY_TYPE_BULLET; entityTypeIndex1 ++ ){
		for(int entityIndex1 =0; entityIndex1 < m_entities[entityTypeIndex1].size(); entityIndex1++){
			Entity* temEntity1 = m_entities[entityTypeIndex1][entityIndex1];
			if(temEntity1 == nullptr){ continue;}
			for( int entityTypeIndex2 = ENTITY_TYPE_PLAYER; entityTypeIndex2 < ENTITY_TYPE_BULLET; entityTypeIndex2++ ) {
				for( int entityIndex2 =0; entityIndex2 < m_entities[entityTypeIndex2].size(); entityIndex2++ ) {
					Entity* temEntity2 = m_entities[entityTypeIndex2][entityIndex2];
					if(temEntity2 ==nullptr){ continue;}
					if(entityTypeIndex1 == entityTypeIndex2 && entityIndex1 == entityIndex2){ continue;}
					CheckCollisionBetweenTwoEntities(temEntity1, temEntity2);
				}

			}
		}
	}

}

void Map::CheckCollisionBetweenTwoEntities( Entity* a, Entity* b )
{
	Vec2 disp = a->m_position - b->m_position;
	float dist = disp.GetLength();
	if(dist > a->m_physicsRadius + b->m_physicsRadius){ return;}
	if(!a->m_isPushedByEntities && ! b->m_isPushedByEntities){ return;}
	if(a->m_doesPushEntities && b->m_isPushedByEntities && !a->m_isPushedByEntities ){
		const Vec2 testingPos = a->m_position;
		PushDiscOutOfDisc2D(b->m_position, b->m_physicsRadius, testingPos, a->m_physicsRadius);
	}
	else if(b->m_doesPushEntities && a->m_isPushedByEntities && !b->m_isPushedByEntities){
		PushDiscOutOfDisc2D(a->m_position, a->m_physicsRadius, b->m_position, b->m_physicsRadius);
	}
	else if(a->m_doesPushEntities && b->m_doesPushEntities && a->m_isPushedByEntities && b->m_isPushedByEntities){
		PushDiscsOutOfEachOther2D(a->m_position, a->m_physicsRadius, b->m_position, b->m_physicsRadius);
	}
}

void Map::CheckPlayerCollision()
{
	Entity* player = m_entities[ENTITY_TYPE_PLAYER][0];//dangerous. should be zero in player list
	if(player == nullptr) return;
	if(!player->m_isPushedByWalls) return;
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

	CheckEntityTileCollisionWithTileCoords( player, downTile );
	CheckEntityTileCollisionWithTileCoords( player, upTile );
	CheckEntityTileCollisionWithTileCoords( player, leftTile );
	CheckEntityTileCollisionWithTileCoords( player, rightTile );

	CheckEntityTileCollisionWithTileCoords( player, leftDownTile );
	CheckEntityTileCollisionWithTileCoords( player, rightUpTile );
	CheckEntityTileCollisionWithTileCoords( player, leftUpTile );
	CheckEntityTileCollisionWithTileCoords( player, rightDownTile );


}

void Map::CheckEnemyTankCollision()
{
	for (int enemyTankIndex =0; enemyTankIndex < m_entities[ENTITY_TYPE_NPC_TANK].size(); enemyTankIndex++){
		Entity* enemyTank = m_entities[ENTITY_TYPE_NPC_TANK][enemyTankIndex];
		if(enemyTank == nullptr) return;
		if(!enemyTank->m_isPushedByWalls) return;
	   //check collide with 8 collides.
		IntVec2 playerTileCoords	=	IntVec2( enemyTank->m_position );
		IntVec2 downTile			=	playerTileCoords+IntVec2( 0, -1 );
		IntVec2 upTile				=	playerTileCoords+IntVec2( 0, 1 );
		IntVec2 leftTile			=	playerTileCoords+IntVec2( -1, 0 );
		IntVec2 rightTile			=	playerTileCoords+IntVec2( 1, 0 );
		IntVec2 leftDownTile		=	playerTileCoords+IntVec2( -1, -1 );
		IntVec2 rightUpTile			=	playerTileCoords+IntVec2( 1, 1 );
		IntVec2 leftUpTile			=	playerTileCoords+IntVec2( -1, 1 );
		IntVec2 rightDownTile		=	playerTileCoords+IntVec2( 1, -1 );
		
		CheckEntityTileCollisionWithTileCoords( enemyTank, downTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, upTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, leftTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, rightTile );
												
		CheckEntityTileCollisionWithTileCoords( enemyTank, leftDownTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, rightUpTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, leftUpTile );
		CheckEntityTileCollisionWithTileCoords( enemyTank, rightDownTile );
	}

	

}

void Map::CheckTurretCollision()
{
	for( int turretIndex =0; turretIndex < m_entities[ENTITY_TYPE_NPC_TURRET].size(); turretIndex++ ) {
		Entity* turret = m_entities[ENTITY_TYPE_NPC_TURRET][turretIndex];
		if( turret == nullptr ) return;
		if( !turret->m_isPushedByWalls ) return;
		//check collide with 8 collides.
		IntVec2 playerTileCoords	=	IntVec2( turret->m_position );
		IntVec2 downTile			=	playerTileCoords+IntVec2( 0, -1 );
		IntVec2 upTile				=	playerTileCoords+IntVec2( 0, 1 );
		IntVec2 leftTile			=	playerTileCoords+IntVec2( -1, 0 );
		IntVec2 rightTile			=	playerTileCoords+IntVec2( 1, 0 );
		IntVec2 leftDownTile		=	playerTileCoords+IntVec2( -1, -1 );
		IntVec2 rightUpTile			=	playerTileCoords+IntVec2( 1, 1 );
		IntVec2 leftUpTile			=	playerTileCoords+IntVec2( -1, 1 );
		IntVec2 rightDownTile		=	playerTileCoords+IntVec2( 1, -1 );

		CheckEntityTileCollisionWithTileCoords( turret, downTile );
		CheckEntityTileCollisionWithTileCoords( turret, upTile );
		CheckEntityTileCollisionWithTileCoords( turret, leftTile );
		CheckEntityTileCollisionWithTileCoords( turret, rightTile );
												
		CheckEntityTileCollisionWithTileCoords( turret, leftDownTile );
		CheckEntityTileCollisionWithTileCoords( turret, rightUpTile );
		CheckEntityTileCollisionWithTileCoords( turret, leftUpTile );
		CheckEntityTileCollisionWithTileCoords( turret, rightDownTile );
	}

}

void Map::CheckBulletCollision()
{
	for(int bulletIndex =0; bulletIndex < m_entities[ENTITY_TYPE_BULLET].size(); bulletIndex ++){
		Entity* temBullet = m_entities[ENTITY_TYPE_BULLET][bulletIndex];
		if(temBullet == nullptr){continue;}
		Vec2 worldPos = temBullet->m_position;
		
		//Check collision with solid tiles
		bool isSolid = isPointInSolid(worldPos);
		if(isSolid){
			if(IsPointInBrick(worldPos)){
				IntVec2 currentPointInTileCoords = TransformFromWorldPosToTilePos( worldPos );
				Vec2 explosionPos = Vec2(currentPointInTileCoords);
				SpawnNewExplosion(explosionPos, BRICK_EXPLOSION_RADIU, BRICK_EXPLOSION_DURATION, true);
				temBullet->Die();
			}
			if(IsPointInWater(worldPos)){
				Bullet* temb = (Bullet*) temBullet;
				temb->m_isInWater = true;
			}
			else {
				temBullet->Die();
			}
		}

		//Check collision with entities
		switch( temBullet->m_faction )
		{
		case ENTITY_FACTION_GOOD:{
			for(int entityTypeIndex = ENTITY_TYPE_NPC_TURRET; entityTypeIndex < ENTITY_TYPE_BOULDER; entityTypeIndex++){
				for(int entityIndex = 0; entityIndex < m_entities[entityTypeIndex].size(); entityIndex++){
					Entity* temEntity = m_entities[entityTypeIndex][entityIndex];
					if(temEntity == nullptr){ continue;}
					if(!temEntity->m_isHitByBullet){continue;}
					if(IsPointInEntity(worldPos,temEntity)){
						temEntity->TakeDamage();
						temBullet->Die();
					}
				}
			}
			break;
		}
		case ENTITY_FACTION_EVIL:{
			for(int entityIndex = 0; entityIndex < m_entities[ENTITY_TYPE_PLAYER].size(); entityIndex++ ){
				Entity* temPlayer = m_entities[ENTITY_TYPE_PLAYER][entityIndex];
				if(temPlayer == nullptr){continue;}
				if(!temPlayer->m_isHitByBullet){continue;}
				if(IsPointInEntity(worldPos, m_entities[ENTITY_TYPE_PLAYER][entityIndex])){
					temPlayer->TakeDamage();
					temBullet->Die();
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

void Map::CheckEntityTileCollisionWithTileCoords( Entity* player, IntVec2 tileCoords )
{
	if(!IsTileCoordsValid(tileCoords)){
		return;
	}

	int tileIndex = GetTileIndexForTileCoords( tileCoords );
	Tile temTile = m_tiles[tileIndex];
	bool isSolid = TileDefinition::s_definitions[ temTile.m_type].GetTileIsSolid();
	if(!isSolid) { return; }
	else {
		AABB2 temAABB2 = temTile.GetBounds();
		PushDiscOutOfAABB2D( player->m_position, player->m_physicsRadius, temAABB2 );
	}
}

void Map::AddEntityToMap( Entity* entity )
{
	if( entity == nullptr){ return ;}
	int entityIndex =0;
	for( entityIndex ; entityIndex < m_entities[entity->m_type].size(); entityIndex++ ){
		if(m_entities[entity->m_type][entityIndex] == nullptr){
			m_entities[entity->m_type][entityIndex] = entity;
			return;
		}
	}
	m_entities[entity->m_type].push_back(entity);
	return;
	
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



void Map::RenderTiles()const
{
	Texture* tileTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->BindTexture(tileTexture);
	g_theRenderer->DrawVertexVector(m_tileVertices);
}



void Map::AddVertsForAABB2D( std::vector<Vertex_PCU>& vertices, AABB2 bound, Rgba8 tineColor, Vec2 uvAtMins, Vec2 uvAtMaxs )
{		//triangle1
	Vertex_PCU temVertex1 =	Vertex_PCU( Vec3( bound.mins, 0.f ), tineColor, uvAtMins );
	Vertex_PCU temVertex2 =	Vertex_PCU( Vec3( bound.maxs.x, bound.mins.y, 0.f ), tineColor, Vec2(uvAtMaxs.x,uvAtMins.y) );
	Vertex_PCU temVertex3 =	Vertex_PCU( Vec3( bound.maxs, 0.f ), tineColor, uvAtMaxs );
		// triangle2
	Vertex_PCU temVertex4 =	Vertex_PCU( Vec3( bound.mins, 0.f ), tineColor, uvAtMins );
	Vertex_PCU temVertex5 =	Vertex_PCU( Vec3( bound.mins.x, bound.maxs.y, 0.f ), tineColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU temVertex6 =	Vertex_PCU( Vec3( bound.maxs, 0.f ), tineColor, uvAtMaxs );

	vertices.push_back(temVertex1);
	vertices.push_back(temVertex2);
	vertices.push_back(temVertex3);
	vertices.push_back(temVertex4);
	vertices.push_back(temVertex5);
	vertices.push_back(temVertex6);

}

void Map::RenderRaycastPoints() const
{
	g_theRenderer->BindTexture(nullptr);
	Vec2 startPoint;
	Vec2 endPoint;
	for(int pointIndex = 0; pointIndex < m_raycastPoints.size(); pointIndex++){
		if(pointIndex % 2 == 0){
			startPoint = m_raycastPoints[pointIndex];
		}
		else if(pointIndex % 2 ==1){
			endPoint = m_raycastPoints[pointIndex];
			g_theRenderer->DrawLine(startPoint,endPoint, LINE_THICK, Rgba8(255,0,0));
		}
	}
}

void Map::Update(float deltaSeconds)
{
	
	CheckIfPlayerExit();
	CheckIfPlayerInLava();
	CheckIfPlayerInMud();
	m_raycastPoints.clear();
	UpdateEntities(deltaSeconds);
	if(!g_theGame->m_debugCamera){
		UpdateCamera();
	}
	CheckCollision();
	
	if(m_deadSceneState != DEAD_SCENE_NULL){
		UpdatePlayerDeadScene(deltaSeconds);
	}
	pushTileVertices();

}

void Map::Render() const
{	
	RenderTiles();
	RenderEntities();
	//RenderRaycastPoints();

	if(m_deadSceneState != DEAD_SCENE_NULL){
		RenderPlayerDeadScene();
	}
	g_theRenderer->EndCamera(*g_camera);

}

