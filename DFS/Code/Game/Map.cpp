#include "Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
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

void Map::SetTileType( IntVec2 tileCoords, TileType type )
{
	int tileIndex = GetTileIndexWithTileCoords( tileCoords );
	Tile& tempTile = m_tiles[tileIndex];
	tempTile.SetTileType( type );
	Vec2 tileWorldPos = tempTile.GetWorldPos();
	AABB2 tileBound = tempTile.GetTileBounds();
	Rigidbody2D* tempRb2D = g_thePhysics->CreateRigidbody( tileWorldPos );
	PolygonCollider2D* tempCol = g_thePhysics->CreatePolyCollider( tileBound, tempRb2D );
	tempRb2D->SetCollider( (Collider2D*)tempCol );
}

void Map::SetTileType( int tileX, int tileY, TileType type )
{
	SetTileType( IntVec2( tileX, tileY), type );
}

IntVec2 Map::GetTileCoordsWithTileIndex( int tileIndex ) const
{
	IntVec2 tileCoords;
	tileCoords.x = tileIndex % m_width;
	tileCoords.y = tileIndex / m_width;
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

void Map::CreateMapFromDefinition()
{
	m_width = m_definition->GetWidth();
	m_height = m_definition->GetHeight();

	static bool isValidMap = false;
	while( !isValidMap ){
		PopulateTiles();		
	}
}

void Map::PopulateTiles()
{
	InitializeTiles();
	GenerateEdgeTiles( 1 );
	RunMapGenSteps();
	PushTileVertices();
}

void Map::InitializeTiles()
{
	int tileNum = m_height * m_width;
	m_tiles.resize( tileNum );

	for( int i = 0; i < tileNum; i++ ) {
		IntVec2 tileCoords = GetTileCoordsWithTileIndex( i );
		m_tiles[i] = Tile( tileCoords, m_definition->GetFillType() );
	}
}

void Map::GenerateEdgeTiles( int edgeThick )
{
	TileType edgeType = m_definition->GetEdgeType();
	for( int tileX = 0; tileX < m_width; tileX++ ) {
		for( int tileY = 0; tileY < edgeThick; tileY++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( tileX, ( m_height - 1 - tileY ), edgeType );
		}
	}

	for( int tileY = 0; tileY < m_height; tileY++ ) {
		for( int tileX = 0; tileX < edgeThick; tileX++ ) {
			SetTileType( tileX, tileY, edgeType );
			SetTileType( ( m_width - 1 - tileX ), tileY, edgeType );
		}
	}
}

void Map::RunMapGenSteps()
{

}

void Map::PushTileVertices()
{
	m_tilesVertices.clear();
	for( int i = 0; i < m_tiles.size(); i++ ) {
		Tile tempTile = m_tiles[i];
		AABB2 tempBound = tempTile.GetTileBounds();
		TileDefinition* tempTileDef = tempTile.GetTileDef();
		AppendVertsForAABB2D( m_tilesVertices, tempBound, tempTileDef->GetTintColor(), tempTileDef->GetSpriteUVAtMins(), tempTileDef->GetSpriteUVAtMaxs() );
	}
}
