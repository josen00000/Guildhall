#include "TileMap.hpp"
#include <limits.h>
#include "Game/Tile.hpp"
#include "Game/Game.hpp"
#include "Game/Portal.hpp"
#include "Game/World.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


extern RenderContext* g_theRenderer;
extern Game* g_theGame;

static float g_debugHeight		= 0.f;
static float g_debugSideLength	= 0.05f;
static float g_debugLineWidth	= 0.05f;
static Rgba8 g_debugColorX		= Rgba8( 255, 0, 0 );
static Rgba8 g_debugColorY		= Rgba8( 0, 255, 0 );
static Rgba8 g_debugLineColor	= Rgba8( 50, 0, 0 );
static Rgba8 g_debugImpactColor = Rgba8( 0, 0, 0 );
// static float g_debugHeight = 1.5f;
// static float g_debugHeight = 1.5f;

TileMap::TileMap( const XmlElement& mapElement, std::string name, World* world )
{
	m_name = name;
	LoadMapDefinitions( mapElement );
	CreateAndPopulateTiles();
	CreateEntities();
	m_world = world;
}

void TileMap::RenderMap() const
{
	Texture* mapTexture = MaterialDefinition::s_sheet.m_diffuseTexture;
	g_theRenderer->EnableDepth( COMPARE_DEPTH_LESS, true );
	g_theRenderer->SetDiffuseTexture( mapTexture );
	g_theRenderer->DrawVertexVector( m_meshes );

// 	if( m_isDebug ) {
// 		DebugDrawRaycast();
// 	}
	Texture* temp = g_theRenderer->CreateOrGetTextureFromFile( "Data/Images/Test_StbiFlippedAndOpenGL.png" );
	g_theRenderer->SetDiffuseTexture( temp );
	RenderEntities();
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
// 	RandomNumberGenerator test = RandomNumberGenerator( m_version );
// 	static Vec3 startPos1	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3) );
// 	static Vec3 endPos1		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3) );
// 	static Vec3 startPos2	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
// 	static Vec3 endPos2		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
// 	static Vec3 startPos3	= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
// 	static Vec3 endPos3		= test.RollRandomVec3InRange( Vec3::ZERO, Vec3( Vec2( m_dimensions ), 3 ) );
// 	MapRaycastResult testResult1 = RayCast( startPos1, endPos1  );
// 	MapRaycastResult testResult2 = RayCast( startPos2, endPos2  );
// 	MapRaycastResult testResult3 = RayCast( startPos3, endPos3  );
}

void TileMap::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	UpdateMeshes();
	UpdateEntities( deltaSeconds );
	CheckCollision();
}

void TileMap::PreparePlayer()
{
	Vec3 cameraPos = Vec3( m_playerStartPos.x, m_playerStartPos.y, 0.5f );
	g_theGame->m_player->Set2DPos( m_playerStartPos );
	g_theGame->SetCameraPos( cameraPos );
	g_theGame->SetCameraYaw( m_playerStartYaw );
}

MapRaycastResult TileMap::RayCast( Vec3 startPos, Vec3 forwardNormal, float maxDistance )
{
	if( m_isDebug ) {
		DebugAddWorldLine( startPos, startPos + forwardNormal * maxDistance, Rgba8::WHITE, 0.f, DEBUG_RENDER_ALWAYS );
	}
	MapRaycastResult result[2];
	for( int i = 0; i < 2; i++ ) {
		result[i].startPosition = startPos;
		result[i].forwardNormal = forwardNormal;
		result[i].maxDistance = maxDistance;
		result[i].impactDistance = (float)INT_MAX;
	}
	TileRaycast( result[0] );
	EntityRaycast( result[1] );
 	
	if( result[0].didImpact ) {
		if( result[1].didImpact ) {
			if( result[0].impactDistance < result[1].impactDistance ) {
				if( m_isDebug ) {
					Vec3 impactPos1 = result[0].impactPosition;
					DebugAddWorldPoint( impactPos1, 0.5f, g_debugImpactColor, DEBUG_RENDER_ALWAYS );
				}
				return result[0];
			}
			else {
				if( m_isDebug ) {
					Vec3 impactPos1 = result[1].impactPosition;
					DebugAddWorldPoint( impactPos1, 0.5f, g_debugImpactColor, DEBUG_RENDER_ALWAYS );
				}
				return result[1];
			}
		}
		else {
			if( m_isDebug && result[0].didImpact ) {
				Vec3 impactPos1 = result[0].impactPosition;
				DebugAddWorldPoint( impactPos1, 0.5f, g_debugImpactColor, DEBUG_RENDER_ALWAYS );
			}
			 return result[0];
		}
	}
	else {
		if( m_isDebug && result[1].didImpact ) {
 			Vec3 impactPos1 = result[1].impactPosition;
			DebugAddWorldPoint( impactPos1, 0.5f, g_debugImpactColor, DEBUG_RENDER_ALWAYS );
		}
		return result[1];
	}
}

MapRaycastResult TileMap::RayCast( Vec3 startPos, Vec3 endPos )
{
	return __super::RayCast(startPos, endPos);
}

void TileMap::CheckCollision()
{
	CheckEntitiesCollision();
	CheckTileCollision();
}

void TileMap::CheckEntitiesCollision()
{
	for( int i = 0; i < m_actors.size(); i++ ) {
		Entity* tempEntityA = m_actors[i];
		for( int j = i; j < m_actors.size(); j++ ) {
			if( i == j ){ continue; }
			Entity* tempEntityB = m_actors[j];
			CheckCollisionBetweenTwoEntities( tempEntityA, tempEntityB );

		}
	}
}

void TileMap::CheckTileCollision()
{
	for( int i = 0; i < m_actors.size(); i++ ) {
		Entity* tempEntity = m_actors[i];
		CheckCollisionBetweenEntityAndTiles( tempEntity );
	}
}

void TileMap::CheckCollisionBetweenTwoEntities( Entity* entityA, Entity* entityB )
{
	Vec2 disp = entityA->Get2DPosition() - entityB->Get2DPosition();
	float dist = disp.GetLength();
	if( dist > entityA->GetRadius() + entityB->GetRadius() ){ return; }
	if( !entityA->GetCanBePushedByEntities() && !entityB->GetCanBePushedByEntities() ){ return; }
	if( entityA->GetCanPushEntities() && entityB->GetCanBePushedByEntities() && !entityA->GetCanBePushedByEntities() ) {
		Vec2 bPos2D = entityB->Get2DPosition();
		PushDiscOutOfDisc2D( bPos2D, entityB->GetRadius(), entityA->Get2DPosition(), entityA->GetRadius() );
		entityB->Set2DPos( bPos2D );
	}
	else if( entityB->GetCanPushEntities() && entityA->GetCanBePushedByEntities() && !entityB->GetCanBePushedByEntities() ) {
		Vec2 aPos2D = entityA->Get2DPosition();
		PushDiscOutOfDisc2D( aPos2D, entityA->GetRadius(), entityB->Get2DPosition(), entityB->GetRadius());
		entityA->Set2DPos( aPos2D );
	}
	else if( entityA->GetCanPushEntities() && entityB->GetCanPushEntities() && entityA->GetCanBePushedByEntities() && entityB->GetCanBePushedByEntities() ) {
		Vec2 aPos2D = entityA->Get2DPosition();
		Vec2 bPos2D = entityB->Get2DPosition();
		PushDiscsOutOfEachOther2D( aPos2D, entityA->GetRadius(), bPos2D, entityB->GetRadius() );
		entityA->Set2DPos( aPos2D );
		entityB->Set2DPos( bPos2D );
	}
}

void TileMap::CheckCollisionBetweenEntityAndTiles( Entity* entityA )
{
	IntVec2 entityTileCoords	= IntVec2( entityA->Get2DPosition() );
	IntVec2 forwardTileCoords	= entityTileCoords + IntVec2( 1, 0 );
	IntVec2 backwardTileCoords	= entityTileCoords + IntVec2( -1, 0 );
	IntVec2 leftTileCoords		= entityTileCoords + IntVec2( 0, 1 );
	IntVec2 rightTileCoords		= entityTileCoords + IntVec2( 0, -1 );

	IntVec2 forwardLeftTileCoords	= entityTileCoords + IntVec2( 1, 1 );
	IntVec2 backwardLeftTileCoords	= entityTileCoords + IntVec2( -1, 1 );
	IntVec2 forwardRightTileCoords	= entityTileCoords + IntVec2( 1, -1 );
	IntVec2 backwardRightTileCoords	= entityTileCoords + IntVec2( -1, -1 );

	CheckEntityTileCollisionWithTileCoords( entityA, forwardTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, backwardTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, leftTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, rightTileCoords );

	CheckEntityTileCollisionWithTileCoords( entityA, forwardLeftTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, backwardLeftTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, forwardRightTileCoords );
	CheckEntityTileCollisionWithTileCoords( entityA, backwardRightTileCoords );
}

void TileMap::CheckEntityTileCollisionWithTileCoords( Entity* entity, IntVec2 tileCoords )
{
	if( !IsTileCoordsValid( tileCoords ) ){ return; }
	if( !IsTileCoordsSolid( tileCoords ) ){ return; }
	
	int tileIndex = GetTileIndexWithCoords( tileCoords );
	AABB2 tileBound = m_tiles[tileIndex].GetBox2D();
	Vec2 entityPos2D = entity->Get2DPosition();
	PushDiscOutOfAABB2D( entityPos2D, entity->GetRadius(), tileBound );
	entity->Set2DPos( entityPos2D );
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
	const XmlElement* entityElement = playerStartElement->NextSiblingElement();
	while( entityElement ) {
		std::string entityType = entityElement->Name();
		if( entityType.compare( "Actor" ) == 0 ) {
			std::string actorName = ParseXmlAttribute( *entityElement, "type", "" );
			auto iter = m_actorList.find( actorName );
			Vec2 startPos	= ParseXmlAttribute( *entityElement, "pos", Vec2::ZERO );
			float yaw		= ParseXmlAttribute( *entityElement, "yaw", 0.f );
			EntityInfo entityInfo;
			entityInfo.pos = startPos;
			entityInfo.yaw = yaw;
			if( iter != m_actorList.end() ) {
				m_actorList[actorName].push_back( entityInfo );
			}
			else {
				std::vector<EntityInfo> entityInfos;
				entityInfos.push_back( entityInfo );
				m_actorList[actorName] = entityInfos;
			}
		}
		else if( entityType.compare( "Portal" ) == 0 ) {
			std::string portalType = ParseXmlAttribute( *entityElement, "type", "" );
			auto iter = m_portalList.find( portalType );
			Vec2 startPos	= ParseXmlAttribute( *entityElement, "pos", Vec2::ZERO );
			float yaw		= ParseXmlAttribute( *entityElement, "yaw", 0.f );
			EntityInfo entityInfo{ startPos, yaw };

			Vec2 targetPos			= ParseXmlAttribute( *entityElement, "targetPos", Vec2::ZERO );
			std::string targetMap	= ParseXmlAttribute( *entityElement, "targetMap", "" );
			PortalInfo porterInfo{ entityInfo, targetPos, targetMap };


			if( iter != m_portalList.end() ) {
				m_portalList[portalType].push_back( porterInfo );
			}
			else {
				std::vector<PortalInfo> porterInfos;
				porterInfos.push_back( porterInfo );
				m_portalList[portalType] = porterInfos;
			}
		}
		else if( entityType.compare( "Projectile" ) == 0 ) {

		}
		entityElement = entityElement->NextSiblingElement();
	}

}

void TileMap::EntityRaycast( MapRaycastResult& result )
{
	result.didImpact = false;

	Vec2 startPosXY = result.startPosition.GetXYVector();
	Vec2 forwardNormalXY = result.forwardNormal.GetXYVector();
	float forwardNormalXYLength = forwardNormalXY.GetLength();
	float maxDistanceXY = result.maxDistance * forwardNormalXYLength;
	forwardNormalXY.Normalize();

	float startPosZ = result.startPosition.z;
	float forwardNormalZLength = abs( result.forwardNormal.z );
	float maxDistanceZ = result.maxDistance * result.forwardNormal.z;


	for( int i = 0; i < m_actors.size(); i++ ) {
		Entity* tempEntity = m_actors[i];
		FloatRange rangeXY = GetEntityXYRaycastDistRange( tempEntity, startPosXY, forwardNormalXY, maxDistanceXY );
		FloatRange rangeZ = GetEntityZRaycastDistRange( tempEntity, startPosZ, maxDistanceZ );

		if( rangeXY.IsFloat() ){ continue; }
		if( rangeZ.IsFloat() && rangeZ.minimum == -1.f ){ continue; }
		
		FloatRange RangeXY_3D;
		FloatRange RangeZ_3D;

		RangeXY_3D = FloatRange( rangeXY.minimum / forwardNormalXYLength, rangeXY.maximum / forwardNormalXYLength );

		if( rangeZ.IsFloat() ) {
			RangeZ_3D = FloatRange( 0.f, 1.f );
		}
		else {
			RangeZ_3D = FloatRange( rangeZ.minimum / forwardNormalZLength, rangeZ.maximum / forwardNormalZLength );
		}

		if( RangeXY_3D.DoesOverlap( RangeZ_3D ) ) {
			// hit the entity;
			result.didImpact = true;
			FloatRange resultRange = FloatRange::GetIntersectRange( RangeXY_3D, RangeZ_3D );

// 			DebugRaycast
// 						if( m_isDebug ) {
// 							Vec3 impactPos1 = result.startPosition + result.forwardNormal * resultRange.minimum;
// 							Vec3 impactPos2 = result.startPosition + result.forwardNormal * resultRange.maximum;
// 							AABB3 debugBox1 = AABB3( impactPos1 - Vec3( g_debugSideLength ), impactPos1 + Vec3( g_debugSideLength ) );
// 							AABB3 debugBox2 = AABB3( impactPos2 - Vec3( g_debugSideLength ), impactPos2 + Vec3( g_debugSideLength ) );
// 							AppendVertsForAABB3D( m_raycastDebugVerts, debugBox1, g_debugColorX, g_theGame->m_convension );
// 							AppendVertsForAABB3D( m_raycastDebugVerts, debugBox2, g_debugColorY, g_theGame->m_convension );
// 							
// 						}

			float tempImpactDist = resultRange.minimum;
			if( tempImpactDist < result.impactDistance ) {
				result.impactDistance = tempImpactDist;
				result.impactEntity = tempEntity;
				result.impactPosition = result.startPosition + result.forwardNormal * result.impactDistance;
				result.impactSurfaceNormal = Vec3::ZERO; // temp
			}
		}
	}
}

void TileMap::TileRaycast( MapRaycastResult& result )
{
	Vec2 startPosXY = result.startPosition.GetXYVector();
	Vec2 forwardNormalXY = result.forwardNormal.GetXYVector();
	float forwardNormalXYLength = forwardNormalXY.GetLength();
	float maxDistanceXY = result.maxDistance * forwardNormalXYLength;
	forwardNormalXY.Normalize();

	float startPosZ = result.startPosition.z;
	float forwardNormalZLength = abs( result.forwardNormal.z );
	float maxDistanceZ = result.maxDistance * result.forwardNormal.z;

	Vec3 tileRaycastNormalAndDistXY = GetTileXYRaycastNormalAndDist( startPosXY, forwardNormalXY, maxDistanceXY );
	float tileRaycastDistXY_3D = tileRaycastNormalAndDistXY.z;
	Vec2 tileRaycastNormalAndDistZ = GetTileZRaycastNormalAndDist( startPosZ, maxDistanceZ );
	float tileRaycastDistZ_3D = tileRaycastNormalAndDistZ.y;


	float tileRaycastDist1 = tileRaycastDistXY_3D / forwardNormalXYLength;
	float tileRaycastDist2 = tileRaycastDistZ_3D / forwardNormalZLength;
	if( tileRaycastDist1 < result.maxDistance || tileRaycastDist2 < result.maxDistance ) {
		result.didImpact = true;
		if( tileRaycastDist1 < tileRaycastDist2 ) {
			result.impactDistance = tileRaycastDist1;
			result.impactPosition = result.startPosition + result.forwardNormal * result.impactDistance;
			result.impactSurfaceNormal = tileRaycastNormalAndDistXY;
			result.impactSurfaceNormal.z = 0;
		}
		else {
			result.impactDistance = tileRaycastDist2;
			result.impactPosition = result.startPosition + result.forwardNormal * result.impactDistance;
			result.impactSurfaceNormal = Vec3( 0.f, 0.f, tileRaycastNormalAndDistZ.x );
		}
	}
	else {
		result.didImpact = false;
	}
}

FloatRange TileMap::GetEntityXYRaycastDistRange( Entity* entity, Vec2 startPosXY, Vec2 forwardNormalXY, float maxDistXY )
{
	Vec2 entityPosXY = entity->Get2DPosition();
	Vec2 entityDisp = entityPosXY - startPosXY;
	Vec2 entityPosXY_raycast = Vec2( ( forwardNormalXY.x * entityDisp.x + forwardNormalXY.y * entityDisp.y ), ( -forwardNormalXY.y * entityDisp.x + forwardNormalXY.x * entityDisp.y ) );
	float entityPhysicsRadius = entity->GetRadius();
	float entityPosHeight = abs( entityPosXY_raycast.y );
	if( entityPosHeight >= entityPhysicsRadius ) {
		return FloatRange( -1.f );
	}

	float deltaX = sqrtf( ( entityPhysicsRadius * entityPhysicsRadius ) - ( entityPosHeight * entityPosHeight ) );
	FloatRange result = FloatRange( entityPosXY_raycast.x - deltaX, entityPosXY_raycast.x + deltaX );
	if( result.IsFloatInRange( maxDistXY ) ) {
		result.maximum = maxDistXY;
	}
	return result;
}

FloatRange TileMap::GetEntityZRaycastDistRange( Entity* entity, float startPosZ, float maxDistZ )
{
	float entityHeight = entity->GetHeight();

	if( startPosZ < 0 || startPosZ > 1 ){ return FloatRange( -1.f ); }

	float endPosZ = startPosZ + maxDistZ;
	if( maxDistZ > 0 ) {
		if( startPosZ > entityHeight ){ return FloatRange( -1.f ); }

		FloatRange result = FloatRange( 0, maxDistZ );
		if( entityHeight > endPosZ ) {
			result.maximum = entityHeight - startPosZ;
		}
		return result;
	}
	else {
		if( endPosZ > entityHeight ){ return FloatRange( -1.f ); }

		FloatRange result = FloatRange( 0, -maxDistZ );
		if( endPosZ < 0  ) {
			result.maximum = startPosZ;
		}
		return result;
	}
}

Vec3 TileMap::GetTileXYRaycastNormalAndDist( Vec2 startPosXY, Vec2 forwardNormalXY, float maxDistXY )
{
// 	OBB2 debugLine = OBB2( Vec2( maxDistXY, g_debugLineWidth), Vec2( startPosXY + ( forwardNormalXY * maxDistXY / 2 ) ), forwardNormalXY );
// 	AppendVertsForOBB2DWithHeight( m_raycastDebugVerts, debugLine, g_debugHeight, g_debugLineColor, g_debugColorX );

	Vec2 raycastDisp = forwardNormalXY * maxDistXY;
	float deltaTPerTileX = 1.f / abs( forwardNormalXY.x );
	float deltaTPerTileY = 1.f / abs( forwardNormalXY.y );
	float currentTImpactInX = 0.f;
	float currentTImpactInY = 0.f;
	int xForwardSign = 0;
	int yForwardSign = 0;
	IntVec2 currentTileCoords = IntVec2( (int)floorf( startPosXY.x ), (int)floorf( startPosXY.y ) );

	if( IsTileCoordsSolid( currentTileCoords ) ){ 
// 		AABB2 debugBox = AABB2( (startPosXY - Vec2( g_debugSideLength )), ( startPosXY + Vec2( g_debugSideLength )) );
// 		AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
		return Vec3::ZERO; 
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
			return Vec3( 0.f, 0.f, maxDistXY );			
		}

		bool isCurrentImpactInX = false;
		if( currentTImpactInX < currentTImpactInY ) {
			isCurrentImpactInX = true;
			currentTileCoords = currentTileCoords + IntVec2( xForwardSign, 0 );
		}
		else {
			currentTileCoords = currentTileCoords + IntVec2( 0, yForwardSign );
		}

// 		debug raycast
// 				if( m_isDebug ) {
// 					Vec2 debugPosXY;
// 					if( isCurrentImpactInX ) {
// 						debugPosXY = startPosXY + ( forwardNormalXY * currentTImpactInX );
// 						/*AppendVertsForAABB2DWithHeight( m_raycastDebugVerts,)*/
// 						AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
// 						AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugColorX, Vec2::ZERO, Vec2::ONE );
// 					}
// 					else {
// 						debugPosXY = startPosXY + ( forwardNormalXY * currentTImpactInY );
// 						AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
// 						AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugColorY, Vec2::ZERO, Vec2::ONE );
// 					}
// 				}

		if( IsTileCoordsSolid( currentTileCoords ) ) {
			if( isCurrentImpactInX ) {
// 				Vec2 debugPosXY = startPosXY + (forwardNormalXY * currentTImpactInX);
// 				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
// 				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
				return Vec3( (float)-xForwardSign, 0.f, currentTImpactInX );
			}
			else {
// 				Vec2 debugPosXY = startPosXY + (forwardNormalXY * currentTImpactInY );
// 				AABB2 debugBox = AABB2( (debugPosXY - Vec2( g_debugSideLength )), (debugPosXY + Vec2( g_debugSideLength )) );
// 				AppendVertsForAABB2DWithHeight( m_raycastDebugVerts, debugBox, g_debugHeight, g_debugImpactColor, Vec2::ZERO, Vec2::ONE );
				return Vec3( 0.f, (float)-yForwardSign, currentTImpactInY );
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

Vec2 TileMap::GetTileZRaycastNormalAndDist( float startPosZ, float maxDistZ )
{
	if( startPosZ <= 0 || startPosZ >= 1 ) {
		return Vec2( 0, 0 );
	}
	
	float endPosZ = startPosZ + maxDistZ;
	if( endPosZ > 0 && endPosZ < 1 ) {
		if( maxDistZ > 0 ) {
			return Vec2( 0, maxDistZ );
		}
		else {
			return Vec2( 0, -maxDistZ );
		}

	}
	
	if( maxDistZ > 0 ){ return  Vec2( -1.f, ( (1 - startPosZ) / maxDistZ ) ); }
	else{ return Vec2( 1.f, ( -startPosZ / maxDistZ ) ); }
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

void TileMap::CreateEntities()
{
	for( auto iter = m_actorList.begin(); iter != m_actorList.end(); ++iter ) {
		std::string actorType = iter->first;
		std::vector<EntityInfo> actorsInfo = iter->second;
		for( int i = 0; i < actorsInfo.size(); i++ ) {
			Entity* tempActor = SpawnNewEntityOfType( actorType );
			tempActor->Set2DPos( actorsInfo[i].pos );
			m_actors.push_back( tempActor );
		}
	}

	for( auto iter = m_portalList.begin(); iter != m_portalList.end(); ++iter ) {
		std::string portalType = iter->first;
		std::vector<PortalInfo>& portalInfos = iter->second;
		for( int i = 0; i < portalInfos.size(); i++ ) {
			Entity* tempPortal = SpawnNewEntityOfType( portalType );
			Portal* portal	= dynamic_cast<Portal*>( tempPortal );
			portal->Set2DPos( portalInfos[i].entityInfo.pos );
			portal->SetTarget2DPosition( portalInfos[i].targetPos );
			portal->SetTargetMapName( portalInfos[i].targetMap );
			m_portals.push_back( portal );
		}
	}
	// create player later.
}

