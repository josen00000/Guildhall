#include "TileMap.hpp"
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/AABB2.hpp"


extern RenderContext* g_theRenderer;
extern Game* g_theGame;


TileMap::TileMap( const XmlElement& mapElement )
{
	LoadMapDefinitions( mapElement );
	CreateAndPopulateTiles();
}

void TileMap::RenderMap() const
{
	Texture* mapTexture = MaterialDefinition::s_sheet.m_diffuseTexture;
	//Sampler* mapSampler = new Sampler( g_theRenderer, SAMPLER_POINT );
	//g_theRenderer->BindSampler( mapSampler );
	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetDiffuseTexture( mapTexture );
	g_theRenderer->DrawVertexVector( m_meshes );
	//delete mapSampler;
}

void TileMap::UpdateMeshes()
{
	m_meshes.clear();
	for( int i = 0; i < m_tiles.size(); i++ ) {
		const Tile& tempTile = m_tiles[i];
		if( tempTile.IsSolid() ) {
			AddVertsForSolidTile( m_meshes, tempTile );
		}
		else {
			AddVertsForNonSolidTile( m_meshes, tempTile );
		}
	}
}

void TileMap::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	UpdateMeshes();
}

void TileMap::PrepareCamera()
{
	Vec3 cameraPos = Vec3( m_playerStartPos.x, m_playerStartPos.y, 0.5f );
	g_theGame->SetCameraPos( cameraPos );
	g_theGame->SetCameraYaw( m_playerStartYaw );
}

Tile TileMap::GetTileWithCoords( IntVec2 tileCoords ) const
{
	int tileIndex = GetTileIndexWithCoords( tileCoords );
	return m_tiles[tileIndex];
}

int TileMap::GetTileIndexWithCoords( IntVec2 tileCoords ) const
{
	int reverseY = m_dimensions.y - 1 - tileCoords.y;
	int tileIndex = tileCoords.x + ( reverseY * m_dimensions.x );
	return tileIndex;
}

bool TileMap::IsTileCoordsValid( IntVec2 tileCoords )
{
	return( tileCoords.x >=0 && tileCoords.x < m_dimensions.x && tileCoords.y >=0 && tileCoords.y < m_dimensions.y );
}

bool TileMap::IsTileCoordsSolid( IntVec2 tileCoords )
{
	if( !IsTileCoordsValid( tileCoords ) ){ return true; }
	Tile tile = GetTileWithCoords( tileCoords );
	return tile.IsSolid();
}

void TileMap::LoadMapDefinitions( const XmlElement& mapElement )
{
	m_version		= ParseXmlAttribute( mapElement, "version", m_version );
	m_dimensions	= ParseXmlAttribute( mapElement, "dimensions", m_dimensions );
	
	const XmlElement* legendElement = mapElement.FirstChildElement();
	const XmlElement* tileElement = legendElement->FirstChildElement();
	while( tileElement ) {
		std::string glyph		= ParseXmlAttribute( *tileElement, "glyph", "" );
		std::string regionType	= ParseXmlAttribute( *tileElement, "regionType", "" );
		m_lengends[glyph] = regionType;
		if( !RegionDefinition::IsRegionTypeValid( regionType ) ) {
			g_theConsole->DebugErrorf( "Region type \"%s\" is not valid in legend!", regionType.c_str() );
			m_lengends[glyph] = RegionDefinition::s_defaultRegion.m_name;
		}
		tileElement = tileElement->NextSiblingElement();
	}

	const XmlElement* mapRowsElement = legendElement->NextSiblingElement();
	const XmlElement* mapRowElement = mapRowsElement->FirstChildElement();
	int temY = 0;
	for( int i = 0; i < m_dimensions.y; i++ ) {
		if( !mapRowElement ) {
			g_theConsole->DebugErrorf( "MapRow size wrong. should be %i instead of %i.", m_dimensions.y, i );
			temY = i;
			break;
		}
		std::string row = ParseXmlAttribute( *mapRowElement, "tiles", "" );
		m_mapRows.push_back( row );
		// charcter valid checking in row
		MapRowCharacterChecking( row );
		if( m_dimensions.x != row.size() ) {
			g_theConsole->DebugErrorf( "MapRow element size Wrong. in line \"%s\".", row.c_str() );
		}
		mapRowElement = mapRowElement->NextSiblingElement();
	}
	if( temY != 0 ) {
		m_dimensions.y = temY;
	}
	int moreRows = 0;
	while( mapRowElement != nullptr ) {
		mapRowElement = mapRowElement->NextSiblingElement();
		moreRows++;
	}

	if( moreRows != 0 ) {
		g_theConsole->DebugErrorf( "Maprow size wrong. Should be %i instead of %i.", m_dimensions.y, m_dimensions.y + moreRows );

	}

	const XmlElement* entitiesElement = mapRowsElement->NextSiblingElement();
	const XmlElement* playerStartElement = entitiesElement->FirstChildElement();
	m_playerStartPos = ParseXmlAttribute( *playerStartElement, "pos", m_playerStartPos );
	m_playerStartYaw = ParseXmlAttribute( *playerStartElement, "yaw", m_playerStartYaw );
}


void TileMap::MapRowCharacterChecking( std::string mapRow )
{
	for( int i = 0; i < mapRow.size(); i++ ) {
		std::string mapRowChar = std::string( 1, mapRow[i] );
		 auto iter = m_lengends.find( mapRowChar );
		 if( iter == m_lengends.end() ) {
			 g_theConsole->DebugErrorf( "\"%c\" legend does not exist", mapRowChar[0] );
		 }
	}
}

void TileMap::CreateAndPopulateTiles()
{
	for( int tileY = m_dimensions.y - 1; tileY >= 0; tileY-- ) {
		for( int tileX = 0; tileX < m_dimensions.x; tileX++ ) {
			std::string regionType = GetRegionTypeFromMapRowsCoords( tileX, tileY );
			RegionDefinition* regionDef = RegionDefinition::s_definitions[regionType];
			m_tiles.emplace_back( IntVec2( tileX, tileY ), regionDef );
		}
	}
}

std::string TileMap::GetRegionTypeFromMapRowsCoords( int tileX, int tileY )
{
	tileY = m_dimensions.y - 1 - tileY;
	char mapRowChar = m_mapRows[tileY][tileX];
	std::string mapRowStr = std::string( 1, mapRowChar );
	std::string regionType = m_lengends[mapRowStr];
	if( regionType.empty() ) {
		regionType = RegionDefinition::s_defaultRegion.m_name;
	}
	return regionType;
}

void TileMap::AddVertsForSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile )
{
	Rgba8 tintColor = Rgba8::WHITE;
	AABB3 box = tile.GetBox();
	Vec3 frontLeftdownPos	= Vec3( box.max.x, box.min.y, box.min.z );
	Vec3 frontRightupPos	= box.max;
	Vec3 frontLeftupPos		= Vec3( box.max.x, box.min.y, box.max.z );
	Vec3 frontRightdownPos	= Vec3( box.max.x, box.max.y, box.min.z );

	Vec3 backLeftdownPos	= Vec3( box.min.x, box.max.y, box.min.z );
	Vec3 backRightupPos		= Vec3( box.min.x, box.min.y, box.max.z );
	Vec3 backLeftupPos		= Vec3( box.min.x, box.max.y, box.max.z );
	Vec3 backRightdownPos	= box.min;

	Vec2 sideUVAtMin;
	Vec2 sideUVAtMax;
	tile.GetSideUVs( sideUVAtMax, sideUVAtMin );


	// left face
	Vertex_PCU leftLeftdown		= Vertex_PCU( backRightdownPos, tintColor, sideUVAtMin );
	Vertex_PCU leftRightup		= Vertex_PCU( frontLeftupPos, tintColor, sideUVAtMax );
	Vertex_PCU leftLeftup		= Vertex_PCU( backRightupPos, tintColor, Vec2( sideUVAtMin.x, sideUVAtMax.y ) );
	Vertex_PCU leftRightdown	= Vertex_PCU( frontLeftdownPos, tintColor, Vec2( sideUVAtMax.x, sideUVAtMin.y ) );

	// right face
	Vertex_PCU rightLeftdown	= Vertex_PCU( frontRightdownPos, tintColor,sideUVAtMin );
	Vertex_PCU rightRightup		= Vertex_PCU( backLeftupPos, tintColor, sideUVAtMax );
	Vertex_PCU rightLeftup		= Vertex_PCU( frontRightupPos, tintColor, Vec2( sideUVAtMin.x, sideUVAtMax.y ) );
	Vertex_PCU rightRightdown	= Vertex_PCU( backLeftdownPos, tintColor, Vec2( sideUVAtMax.x, sideUVAtMin.y ) );

	// front face
	Vertex_PCU frontLeftdown	= Vertex_PCU( frontLeftdownPos, tintColor,sideUVAtMin );
	Vertex_PCU frontRightup		= Vertex_PCU( frontRightupPos, tintColor, sideUVAtMax );
	Vertex_PCU frontLeftup		= Vertex_PCU( frontLeftupPos, tintColor, Vec2( sideUVAtMin.x, sideUVAtMax.y ) );
	Vertex_PCU frontRightdown	= Vertex_PCU( frontRightdownPos, tintColor, Vec2( sideUVAtMax.x, sideUVAtMin.y ) );

	// back face
	Vertex_PCU backLeftdown		= Vertex_PCU( backLeftdownPos, tintColor, sideUVAtMin );
	Vertex_PCU backRightup		= Vertex_PCU( backRightupPos, tintColor, sideUVAtMax );
	Vertex_PCU backLeftup		= Vertex_PCU( backLeftupPos, tintColor, Vec2( sideUVAtMin.x, sideUVAtMax.y ) );
	Vertex_PCU backRightdown	= Vertex_PCU( backRightdownPos, tintColor, Vec2( sideUVAtMax.x, sideUVAtMin.y ) );

	IntVec2 leftTileCoords = tile.m_tileCoords + IntVec2( 0, -1 );
	IntVec2 rightTileCoords = tile.m_tileCoords + IntVec2( 0, 1 );
	IntVec2 frontTileCoords = tile.m_tileCoords + IntVec2( 1, 0 );
	IntVec2 backTileCoords = tile.m_tileCoords + IntVec2( -1, 0 );
	

	// front
	
	if( !IsTileCoordsSolid( frontTileCoords ) ) {
		meshes.push_back( frontLeftdown );
		meshes.push_back( frontRightdown );
		meshes.push_back( frontRightup );
	
		meshes.push_back( frontLeftdown );
		meshes.push_back( frontRightup );
		meshes.push_back( frontLeftup );
	}

	//back triangles
 	if( !IsTileCoordsSolid( backTileCoords ) ) {
 		meshes.push_back( backLeftdown );
 		meshes.push_back( backRightdown );
 		meshes.push_back( backRightup );
 
 		meshes.push_back( backLeftdown );
 		meshes.push_back( backRightup );
 		meshes.push_back( backLeftup );
 	}

	// left triangles
	if( !IsTileCoordsSolid( leftTileCoords ) ){
		meshes.push_back( leftLeftdown );
		meshes.push_back( leftRightdown );
		meshes.push_back( leftRightup );

		meshes.push_back( leftLeftdown );
		meshes.push_back( leftRightup );
		meshes.push_back( leftLeftup );
	}

	// right triangles
	if( !IsTileCoordsSolid( rightTileCoords ) ){
		meshes.push_back( rightLeftdown );
		meshes.push_back( rightRightdown );
		meshes.push_back( rightRightup );

		meshes.push_back( rightLeftdown );
		meshes.push_back( rightRightup );
		meshes.push_back( rightLeftup );
	}
}

void TileMap::AddVertsForNonSolidTile( std::vector<Vertex_PCU>& meshes, const Tile& tile )
{
	Rgba8 tintColor = Rgba8::WHITE;
	AABB3 box = tile.GetBox();
	Vec3 frontLeftdownPos	= Vec3( box.max.x, box.min.y, box.min.z );
	Vec3 frontRightupPos	= box.max;
	Vec3 frontLeftupPos		= Vec3( box.max.x, box.min.y, box.max.z );
	Vec3 frontRightdownPos	= Vec3( box.max.x, box.max.y, box.min.z );

	Vec3 backLeftdownPos	= Vec3( box.min.x, box.max.y, box.min.z );
	Vec3 backRightupPos		= Vec3( box.min.x, box.min.y, box.max.z );
	Vec3 backLeftupPos		= Vec3( box.min.x, box.max.y, box.max.z );
	Vec3 backRightdownPos	= box.min;

	Vec2 floorUVAtMax;
	Vec2 floorUVAtMin;
	Vec2 ceilingUVAtMax;
	Vec2 ceilingUVAtMin;

	tile.GetCeilingUVs( ceilingUVAtMax, ceilingUVAtMin );
	tile.GetFloorUVs( floorUVAtMax, floorUVAtMin );
	// top face
	Vertex_PCU topLeftdown		= Vertex_PCU( backRightupPos, tintColor, Vec2(ceilingUVAtMax.x, ceilingUVAtMin.y ));
	Vertex_PCU topRightup		= Vertex_PCU( frontRightupPos, tintColor, Vec2( ceilingUVAtMin.x, ceilingUVAtMax.y ));
	Vertex_PCU topLeftup		= Vertex_PCU( backLeftupPos, tintColor, Vec2( ceilingUVAtMax.x, ceilingUVAtMax.y ) );
	Vertex_PCU topRightdown		= Vertex_PCU( frontLeftupPos, tintColor, Vec2( ceilingUVAtMin.x, ceilingUVAtMin.y ) );

	// bottom face
	Vertex_PCU bottomLeftdown	= Vertex_PCU( backLeftdownPos, tintColor, Vec2( floorUVAtMin.x, floorUVAtMax.y ) );
	Vertex_PCU bottomRightup	= Vertex_PCU( frontLeftdownPos, tintColor, Vec2( floorUVAtMax.x, floorUVAtMin.y ) );
	Vertex_PCU bottomLeftup		= Vertex_PCU( backRightdownPos, tintColor, Vec2( floorUVAtMin.x, floorUVAtMin.y ) );
	Vertex_PCU bottomRightdown	= Vertex_PCU( frontRightdownPos, tintColor, Vec2( floorUVAtMax.x, floorUVAtMax.y ) );

	//top triangles
 	meshes.push_back( topLeftdown );
 	meshes.push_back( topRightup );
 	meshes.push_back( topRightdown );
 
 	meshes.push_back( topLeftdown );
 	meshes.push_back( topLeftup );
 	meshes.push_back( topRightup );

	// bottom triangles
	meshes.push_back( bottomLeftdown );
	meshes.push_back( bottomRightup );
	meshes.push_back( bottomRightdown );

	meshes.push_back( bottomLeftdown );
	meshes.push_back( bottomLeftup );
	meshes.push_back( bottomRightup );

}

