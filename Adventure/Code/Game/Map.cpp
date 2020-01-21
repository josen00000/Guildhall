#include "Map.hpp"
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/World.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/Mutator.hpp"
#include "Game/Worm.hpp"
#include "Game/MapGenStep.hpp"
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

Map::Map( World* world, std::string name, MapDefinition* definition )
	:m_world(world)
	,m_name(name)
	,m_definition(definition)
{
	m_width = m_definition->m_width;
	m_height = m_definition->m_height;
	CreateMap();
}


void Map::CreateMap()
{
	static bool isValidMap = false;
	while(!isValidMap){
		PopulateTiles();
		isValidMap = true;
	}
	PushTileVertices();
}




IntVec2 Map::GetRandomValidTilePositionInMap()
{
	int edgeThick = m_definition->m_edgeThick;
	int temX = m_rng.RollRandomIntInRange( 0, m_width - edgeThick );
	int temY = m_rng.RollRandomIntInRange( 0, m_height - edgeThick );
	return IntVec2(temX, temY);
}


void Map::CreateActors()
{
	//Actor* tempActor = new Actor();
}


bool Map::IsTileInside( const IntVec2& tileCoords ) const
{
	if(!IsTileCoordsValid( tileCoords )){ return false; }
	if(IsTileInEdge( tileCoords )){ return false; }
	return true; //temp
}


bool Map::IsTileInType( const IntVec2& tileCoords, std::string tileType ) const
{
	if( !IsTileInside( tileCoords ) ){ return false; }
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	Tile temTile = m_tiles[tileIndex];
	if( temTile.m_type == tileType ){
		return true;
	}
	else{
		return false;
	}
}


void Map::PopulateTiles()
{
	InitializeTiles();
	GenerateEdgeTiles(m_definition->m_edgeThick);
 	RunMapGenSteps();
	//GenerateStartTiles();
	//GenerateExitTiles();


}


void Map::RunMapGenSteps()
{
	std::vector<MapGenStep*>& runSteps = m_definition->m_mapGenSteps; 
	for(int stepIndex = 0; stepIndex < runSteps.size(); stepIndex++ ){
		MapGenStep* tempStep = runSteps[stepIndex];
 		tempStep->RunStep( this );
	}
}

void Map::InitializeTiles()
{
	int tileNum = m_height * m_width;
	m_tiles.resize(tileNum);
	m_tileMapAttributes.resize(tileNum);

	for(int tileIndex = 0; tileIndex < tileNum; ++tileIndex){
		IntVec2 tileCoords = GetTileCoordsWithTileIndex(tileIndex);
		m_tiles[tileIndex] = Tile(tileCoords, m_definition->m_fillTileType);
		m_tileMapAttributes[tileIndex] = TileAttributeInMap(tileIndex);
	}
}

void Map::GenerateEdgeTiles( int edgeThick )
{
	std::string edgeType = m_definition->m_edgeTileType;
	for(int tileX = 0; tileX < m_width; ++tileX ){
		for(int tileY = 0; tileY < edgeThick; ++tileY){
			SetTileType(tileX, m_height - 1 - tileY, edgeType);
			SetTileType(tileX, tileY, edgeType );
		}
	}

	for( int tileY = 0; tileY < m_height; ++tileY ) {
		for( int tileX = 0; tileX < edgeThick; ++tileX ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( m_width - 1 - tileX, tileY, edgeType );
		}
	}
}

void Map::Render() const
{
	RenderTiles();
}


void Map::Update( float deltaSeconds )
{
	UNUSED( deltaSeconds );
	UpdateMousePosInCamera( *g_camera );
	
}


void Map::DestroyEntities()
{

}


IntVec2 Map::GetMapDimension() const
{
	IntVec2 result = IntVec2( m_width, m_height );
	return result;
}

std::vector<Tile>& Map::GetMapTiles()
{
	return m_tiles;
}

std::vector<std::string> Map::GetDisplayInfo() const
{
	std::vector<std::string> result;
	for(int tileIndex = 0; tileIndex < m_tiles.size(); tileIndex++ ){
		AABB2 bound = m_tiles[tileIndex].GetBounds();
		if(bound.IsPointInside( m_mousePos )){
			std::string tileString = GetDisplayTileInfo( tileIndex );
			result.push_back( tileString );

		}
	}
	return result;

}

IntVec2 Map::GetTileCoordsWithTileIndex( int index ) const
{
	int tileX = index % m_width;
	int tileY = index / m_width;
	if(!IsTileCoordsValid(IntVec2(tileX, tileY))){
		ERROR_AND_DIE( Stringf("tile coords x = %i, y = %i not valid ", tileX, tileY));
	}
	return IntVec2(tileX, tileY);
}


int Map::GetTileIndexWithTileCoords( const IntVec2& tileCoords) const
{
	if(!IsTileCoordsValid( tileCoords )){
		ERROR_RECOVERABLE( Stringf( "tile coords x = %i, y = %i not valid ", tileCoords.x, tileCoords.y ) );
	}
	int tileIndex = ( tileCoords.y * m_width ) + tileCoords.x;
	return tileIndex;
}

Tile& Map::GetTileWithTileIndex( int tileIndex )
{
	return m_tiles[tileIndex];
}

Tile& Map::GetTileWithTileCoords( const IntVec2& tileCoords )
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return m_tiles[tileIndex];
}

bool Map::IsTileCoordsValid( const IntVec2& tileCoords ) const
{
	if(( tileCoords.x >= m_width || tileCoords.x < 0 ) || ( tileCoords.y < 0 || tileCoords.y >= m_height)){
		return false;
	}
	else{
		return true;
	}
}

bool Map::IsTileContainsTags( const IntVec2& tileCoords, Strings tags ) const
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	return m_tiles[tileIndex].IsTagsExist( tags );

}


void Map::SetTileType( int tileX, int tileY, std::string tileType )
{
	IntVec2 tileCoords = IntVec2( tileX, tileY );
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_AND_DIE( Stringf( "the tile coords(x = %i, y = %i) is not valid in SetTileType.", tileCoords.x, tileCoords.y ) );
		return;
	}
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	m_tiles[tileIndex].SetTileType( tileType );
}


void Map::SetTileTags( int tileX, int tileY, Strings tags )
{
	IntVec2 tileCoords = IntVec2( tileX, tileY );
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_AND_DIE( Stringf( "the tile coords(x = %i, y = %i) is not valid in SetTileType.", tileCoords.x, tileCoords.y ) );
		return;
	}
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	m_tiles[tileIndex].SetTileTags( tags );
}


void Map::SetTileHeat( int tileX, int tileY, FloatRange heatRange )
{
	IntVec2 tileCoords = IntVec2( tileX, tileY );
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_AND_DIE( Stringf( "the tile coords(x = %i, y = %i) is not valid in SetTileType.", tileCoords.x, tileCoords.y ) );
		return;
	}
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	float heat = heatRange.GetRandomInRange( m_rng );
	m_tiles[tileIndex].SetTileHeat( heat );
}


void Map::OperateTileHeat( int tileX, int tileY, int operation, FloatRange operateRange )
{
	IntVec2 tileCoords = IntVec2( tileX, tileY );
	if( !IsTileCoordsValid( tileCoords ) ) {
		ERROR_AND_DIE( Stringf( "the tile coords(x = %i, y = %i) is not valid in SetTileType.", tileCoords.x, tileCoords.y ) );
		return;
	}
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	float operateHeat = operateRange.GetRandomInRange( m_rng );
	if( operation == 0 ){
		m_tiles[tileIndex].AddTileHeat( operateHeat );
	}
	else if( operation == 1){
		m_tiles[tileIndex].SubtractTileHeat( operateHeat );
	}
	else if( operation == 2 ){
		m_tiles[tileIndex].MultiplyTileHeat( operateHeat );
	}
	else/* operation == 3*/{
		m_tiles[tileIndex].DivideTileHeat( operateHeat );
	}
}

bool Map::IsTileInEdge( const IntVec2& tileCoords ) const
{
	int tileIndex = GetTileIndexWithTileCoords(tileCoords);
	if(m_tileMapAttributes[tileIndex].m_isEdge){
		return true;
	}
	else{
		return false;
	}
}

void Map::PushTileVertices()
{
	m_tilesVertices.clear();
	for( int tileIndex = 0; tileIndex < m_tiles.size(); ++tileIndex ){
		Tile tempTile = m_tiles[tileIndex];
		AABB2 tempbound = tempTile.GetBounds();
		Rgba8 tintColor = Rgba8( 255,255,255 ); // temp tine color.
		const TileDefinition* tempTileDef = tempTile.GetTileDef();
		AddVerticesForAABB2( m_tilesVertices, tempbound, tintColor, tempTileDef->m_uvAtMins, tempTileDef->m_uvAtMaxs );
	}
}

void Map::RenderTiles() const
{
	Texture* tempTexture = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Terrain_8x8.png" );
	g_theRenderer->BindTexture( tempTexture );
	g_theRenderer->DrawVertexVector(m_tilesVertices);
}

void Map::AddVerticesForAABB2( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2 minUV, const Vec2 maxUV )
{
	//triangle 1
	Vertex_PCU leftdown		= Vertex_PCU(bound.mins, tintColor, minUV);
	Vertex_PCU leftup		= Vertex_PCU(Vec2(bound.mins.x, bound.maxs.y), tintColor, Vec2(minUV.x, maxUV.y));
	Vertex_PCU rightUp		= Vertex_PCU(bound.maxs, tintColor, maxUV);
	Vertex_PCU rightdown	= Vertex_PCU( Vec2( bound.maxs.x, bound.mins.y ), tintColor, Vec2( maxUV.x, minUV.y ) );


	vertices.push_back( leftdown );
	vertices.push_back( leftup );
	vertices.push_back( rightUp );
	vertices.push_back( leftdown);
	vertices.push_back( rightdown );
	vertices.push_back( rightUp );


}

void Map::UpdateMousePosInCamera( const Camera& camera )
{
	m_mousePos = g_theInputSystem->GetNormalizedMousePosInCamera( camera );

}

std::string Map::GetDisplayTileInfo( int tileIndex ) const
{
	std::string result;
	result = "index: " + std::to_string( tileIndex ) + ". Type: " + m_tiles[tileIndex].m_tileDef->m_name + ". Tags:" + m_tiles[tileIndex].GetTileTagsInString();
	return result;
}
