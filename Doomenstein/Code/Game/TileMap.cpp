#include "TileMap.hpp"
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


extern RenderContext* g_theRenderer;
extern Game* g_theGame;

static float g_debugHeight		= 1.5f;
static float g_debugSideLength	= 0.05f;
static float g_debugLineWidth	= 0.05f;
static Rgba8 g_debugColorX		= Rgba8( 255, 0, 0 );
static Rgba8 g_debugColorY		= Rgba8( 0, 255, 0 );
static Rgba8 g_debugLineColor	= Rgba8( 50, 0, 0 );
static Rgba8 g_debugImpactColor = Rgba8( 0, 0, 0 );
// static float g_debugHeight = 1.5f;
// static float g_debugHeight = 1.5f;

TileMap::TileMap( const XmlElement& mapElement )
{
	LoadMapDefinitions( mapElement );
	CreateAndPopulateTiles();
}

void TileMap::RenderMap() const
{
	Texture* mapTexture = MaterialDefinition::s_sheet.m_diffuseTexture;
	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetDiffuseTexture( mapTexture );
	g_theRenderer->DrawVertexVector( m_meshes );

	if( m_isDebug ) {
		DebugDrawRaycast();
	}
}

void TileMap::UpdateMeshes()
{
	m_meshes.clear();
	m_raycastDebugVerts.clear();
	for( int i = 0; i < m_tiles.size(); i++ ) {
		const Tile& tempTile = m_tiles[i];
		if( tempTile.IsSolid() ) {
			AddVertsForSolidTile( m_meshes, tempTile );
		}
		else {
			AddVertsForNonSolidTile( m_meshes, tempTile );
		}
	}
	RandomNumberGenerator test = RandomNumberGenerator( m_version );
	static Vec3 startPos1	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3) );
	static Vec3 endPos1		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3) );
	static Vec3 startPos2	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
	static Vec3 endPos2		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
	static Vec3 startPos3	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
	static Vec3 endPos3		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
	MapRaycastResult testResult1 = RayCast( startPos1, endPos1  );
	MapRaycastResult testResult2 = RayCast( startPos2, endPos2  );
	MapRaycastResult testResult3 = RayCast( startPos3, endPos3  );
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

MapRaycastResult TileMap::RayCast( Vec3 startPos, Vec3 forwardNormal, float maxDistance )
{
	MapRaycastResult result[4];
	for( int i = 0; i < 4; i++ ) {
		result[i].startPosition = startPos;
		result[i].forwardNormal = forwardNormal;
		result[i].maxDistance = maxDistance;
	}

	Vec2 startPosXY = startPos.GetXYVector();
	Vec2 forwardNormalXY = forwardNormal.GetXYVector();
	float maxDistanceXY = maxDistance * forwardNormalXY.GetLength();
	forwardNormalXY.Normalize();

	float startPosZ = startPos.z;
	float maxDistanceZ = maxDistance * forwardNormal.z;

	//Capsule2 raycastLine = Capsule2( startPos.GetXYVector(), ( startPos + forwardNormal * maxDistance )
	float tileRaycastDistXY = GetTileXYRaycastDist( startPosXY, forwardNormalXY, maxDistanceXY );
	float tileRaycastDistZ = GetTileZRaycastDist( startPosZ, maxDistanceZ );
// 	EntityXYRaycast( result[2] );
// 	EntityZRaycast( result[3] );
// 	IntegrateResults( result );
	return result[0];
}

MapRaycastResult TileMap::RayCast( Vec3 startPos, Vec3 endPos )
{
	return __super::RayCast(startPos, endPos);
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


void TileMap::EntityXYRaycast( MapRaycastResult& result )
{

}

void TileMap::EntityZRaycast( MapRaycastResult& result )
{

}

// float TileMap::GetTileXYRaycastDist( MapRaycastResult& result )
// {
// 	// Calculate first check x and y
// 	// while maxdistance < check x and check y keep check and update
// 	Vec2 forwardNormalXY = Vec2( result.forwardNormal.x, result.forwardNormal.y );
// 	forwardNormalXY.Normalize();
// 
// 	Vec2 deltaPos = forwardNormalXY * result.maxDistance;
// 	Vec2 lastPos = result.startPosition + forwardNormalXY * result.maxDistance;
// 	float deltaTPerTileX = result.maxDistance / abs( forwardNormalXY.x * result.maxDistance );
// 	float deltaTPerTileY = result.maxDistance / abs( forwardNormalXY.y * result.maxDistance );
// 
// 	float currentTImpactInX = 0;
// 	float currentTImpactInY = 0;
// 	int xForwardSign = 0;
// 	int yForwardSign = 0;
// 	IntVec2 currentTileCoords = IntVec2( (int)floorf( result.startPosition.x ), (int)floorf( result.startPosition.y ) );
// 
// 	if( forwardNormalXY.x > 0 ) {
// 		xForwardSign = 1;
// 		currentTImpactInX = ( currentTileCoords.x + 1 - result.startPosition.x ) * deltaTPerTileX;
// 	}
// 	else {
// 		xForwardSign = -1;
// 		currentTImpactInX = ( result.startPosition.x - currentTileCoords.x ) * deltaTPerTileX;
// 	}
// 	if( forwardNormalXY.y > 0 ) {
// 		yForwardSign = 1;
// 		currentTImpactInY = ( currentTileCoords.y + 1 - result.startPosition.y ) * deltaTPerTileY;
// 	}
// 	else {
// 		yForwardSign = -1;
// 		currentTImpactInY = ( result.startPosition.y - currentTileCoords.y ) * deltaTPerTileY;
// 	}
// 
// 	while( true ) {
// 		if( currentTImpactInY > result.maxDistance && currentTImpactInX > result.maxDistance ) { 
// 			result.didImpact = false;
// 			break; 
// 		}
// 		bool isImpactInX = false;
// 		if( currentTImpactInX < currentTImpactInY ) {
// 			isImpactInX = true;
// 			currentTileCoords = currentTileCoords + IntVec2( xForwardSign, 0 );
// 		}
// 		else {
// 			currentTileCoords = currentTileCoords + IntVec2( yForwardSign, 0 );
// 		}
// 
// 		if( m_isDebug ) {
// 				Vec3 debugPos;
// 			if( isImpactInX ) {
// 				debugPos = result.startPosition + Vec3( forwardNormalXY, 0.f ) * currentTImpactInX;
// 			}
// 			else {
// 				debugPos = result.startPosition + Vec3( forwardNormalXY, 0.f ) * currentTImpactInY;
// 			}
// 				Vec2 debugPosXY = debugPos.GetXYVector();
// 				AABB2 debugBox = AABB2(( debugPosXY - Vec2( g_debugSideLength )), ( debugPosXY + Vec2( g_debugSideLength )));
// 				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugColor, Vec2::ZERO, Vec2::ONE );
// 		}
// 		if( IsTileCoordsSolid( currentTileCoords ) ){ 
// 			result.didImpact = true;				
// 			if( isImpactInX ) {
// 				result.impactPosition = result.startPosition + Vec3( forwardNormalXY, 0.f ) * currentTImpactInX ;			
// 				result.impactDistance = currentTImpactInX;		
// 				result.impactSurfaceNormal = Vec3( (float)-xForwardSign, 0.f, 0.f );
// 			}
// 			else {
// 				result.impactPosition = result.startPosition + Vec3( forwardNormalXY, 0.f ) * currentTImpactInY;
// 				result.impactDistance = currentTImpactInY;
// 				result.impactSurfaceNormal = Vec3( (float)-yForwardSign, 0.f, 0.f );
// 			}
// 			break; 
// 		}
// 		if( isImpactInX ) {
// 			currentTImpactInX += deltaTPerTileX;
// 		}
// 		else {
// 			currentTImpactInY += deltaTPerTileY;
// 		}
// 	}
// 	if( result.didImpact ) {
// 		Vec2 impactPos = result.impactPosition.GetXYVector();
// 		AABB2 debugBox{ ( impactPos - Vec2( g_debugSideLength ) ), ( impactPos + Vec2( g_debugSideLength ) ) };
// 		AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor );
// 	}
// }

float TileMap::GetTileXYRaycastDist( Vec2 startPosXY, Vec2 forwardNormalXY, float maxDistXY )
{
	OBB2 debugLine = OBB2( Vec2( maxDistXY, g_debugLineWidth), Vec2( startPosXY + ( forwardNormalXY * maxDistXY / 2 ) ), forwardNormalXY );
	AppendVertsForOBB2DWithHeight( m_raycastDebugVerts, debugLine, g_debugHeight, g_debugLineColor, g_debugColorX );

	Vec2 raycastDisp = forwardNormalXY * maxDistXY;
	float deltaTPerTileX = 1.f / abs( forwardNormalXY.x );
	float deltaTPerTileY = 1.f / abs( forwardNormalXY.y );
	float currentTImpactInX = 0.f;
	float currentTImpactInY = 0.f;
	int xForwardSign = 0;
	int yForwardSign = 0;
	IntVec2 currentTileCoords = IntVec2( (int)floorf( startPosXY.x ), (int)floorf( startPosXY.y ) );

	if( IsTileCoordsSolid( currentTileCoords ) ){ 
		AABB2 debugBox = AABB2( (startPosXY - Vec2( g_debugSideLength )), ( startPosXY + Vec2( g_debugSideLength )) );
		AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
		return 0.f; 
	}


	if( forwardNormalXY.x > 0 ) {
		xForwardSign = 1;
		currentTImpactInX = ( currentTileCoords.x + 1 - startPosXY.x ) * deltaTPerTileX;
	}
	else {
		xForwardSign = -1;
		currentTImpactInX = ( startPosXY.x - currentTileCoords.x ) * deltaTPerTileX;
	}
	if( forwardNormalXY.y > 0 ) {
		yForwardSign = 1;
		currentTImpactInY = ( currentTileCoords.y + 1 - startPosXY.y ) * deltaTPerTileY;
	}
	else {
		yForwardSign = -1;
		currentTImpactInY = ( startPosXY.y - currentTileCoords.y ) * deltaTPerTileY;
	}

	while( true ) {
		if( currentTImpactInY > maxDistXY && currentTImpactInX > maxDistXY ) {
			return maxDistXY;			
		}

		bool isCurrentImpactInX = false;
		if( currentTImpactInX < currentTImpactInY ) {
			isCurrentImpactInX = true;
			currentTileCoords = currentTileCoords + IntVec2( xForwardSign, 0 );
		}
		else {
			currentTileCoords = currentTileCoords + IntVec2( 0, yForwardSign );
		}

		// debug raycast
		if( m_isDebug ) {
			Vec2 debugPosXY;
			if( isCurrentImpactInX ) {
				debugPosXY = startPosXY + ( forwardNormalXY * currentTImpactInX );
				/*AppendVertsForAABB2DWithHeight( m_raycastDebugVerts,)*/
				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugColorX, Vec2::ZERO, Vec2::ONE );
			}
			else {
				debugPosXY = startPosXY + ( forwardNormalXY * currentTImpactInY );
				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugColorY, Vec2::ZERO, Vec2::ONE );
			}
		}

		if( IsTileCoordsSolid( currentTileCoords ) ) {
			if( isCurrentImpactInX ) {
				Vec2 debugPosXY = startPosXY + (forwardNormalXY * currentTImpactInX);
				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
				return currentTImpactInX;
			}
			else {
				Vec2 debugPosXY = startPosXY + (forwardNormalXY * currentTImpactInX);
				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
				return currentTImpactInY;
			}
		}

		if( isCurrentImpactInX ) {
			currentTImpactInX += deltaTPerTileX;
		}
		else {
			currentTImpactInY += deltaTPerTileY;
		}
	}
}

float TileMap::GetTileZRaycastDist( float startPosZ, float maxDistZ )
{
	if( startPosZ <= 0 || startPosZ >=1 ) {
		return 0;
	}
	
	float endPosZ = startPosZ + maxDistZ;
	if( endPosZ > 0 || endPosZ < 1 ) {
		return maxDistZ;
	}
	
	if( maxDistZ > 0 ){ return (1 - startPosZ); }
	else{ return (startPosZ - 0); }
}

MapRaycastResult TileMap::GetIntegrateResults( MapRaycastResult* results )
{
	return MapRaycastResult();
}

void TileMap::DebugDrawRaycast() const
{
	g_theRenderer->SetDiffuseTexture( nullptr ); 
	g_theRenderer->SetCullMode( RASTER_CULL_NONE );
	g_theRenderer->DisableDepth();
	g_theRenderer->DrawVertexVector( m_raycastDebugVerts );
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

