#include "Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/World.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"

extern Game* g_theGame;

Map::~Map()
{

}

void Map::RenderEntities() const
{
// 	for( int i = 0; i < m_actors.size(); i++ ) {
// 		m_actors[i]->Render();
// 	}
// 	
// 	for( int i = 0; i < m_portals.size(); i++ ) {
// 		m_portals[i]->Render();
// 	}
}

void Map::CheckCollision()
{

}

void Map::PreparePlayer()
{

}

MapRaycastResult Map::RayCast( Vec3 startPos, Vec3 endPos )
{
	Vec3 raycastDisp = endPos - startPos;
	Vec3 forwardNormal = raycastDisp.GetNormalized();
	float maxdist = raycastDisp.GetLength();

	return RayCast( startPos, forwardNormal, maxdist );
}

void Map::UpdateEntities( float deltaSeconds )
{
// 	for( int i = 0; i < m_actors.size(); i++ ) {
// 		m_actors[i]->Update( deltaSeconds );
// 	}
// 	
// 	Entity* player = g_theGame->GetPlayer();
// 
// 	for( int i = 0; i < m_portals.size(); i++ ) {
// 		Vec2 portalPos = m_portals[i]->Get2DPosition();
// 		
// 		for( int j = 0; j < m_actors.size(); j++ ) {
// 			Vec2 actorPos = m_actors[j]->Get2DPosition();
// 
// 			float distance = GetDistance2D( actorPos, portalPos );	 
// 
// 			if( distance < m_actors[j]->GetRadius() + m_portals[i]->GetRadius() ) {
// 				Portal* tempPortal = dynamic_cast<Portal*>(m_portals[i]);
// 				TeleportToTargetMap( tempPortal->GetTargetMapName(), tempPortal->GetTarget2DPosition(), m_actors[j] );
// 			}
// 
// 		}
// 		Vec2 playerPos = player->Get2DPosition();
// 		Vec2 portalPos = m_portals[i]->Get2DPosition();
// 		
// 		float distance = GetDistance2D( playerPos, portalPos );
// 		if( distance < player->GetRadius() + m_portals[i]->GetRadius() ) {
// 			Portal* tempPortal = dynamic_cast<Portal*>( m_portals[i] );
// 			TeleportToTargetMap( tempPortal->GetTargetMapName(), tempPortal->GetTarget2DPosition() );
// 		}
// 	}
// 	if( player ) {
// 
// 	}
}

void Map::TeleportToTargetMap( std::string targetMap, Vec2 target2DPos, Entity* actor )
{
	if( !targetMap.empty() && actor->GetIsPlayer() ) {
		m_world->LoadMap( targetMap );
	}
	actor->Set2DPos( target2DPos );
}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	const EntityDefinition& entityDef = EntityDefinition::s_definitions[entityDefName];
	return SpawnNewEntityOfType( entityDef );
}

Entity* Map::SpawnNewEntityOfType( EntityDefinition const& entityDef )
{
	Entity* result = nullptr;
	if( entityDef.m_type.compare( "Actor" ) == 0 ){
		result = new Actor( entityDef );
	}
	else if( entityDef.m_type.compare( "Projectile" ) == 0 ) {
		result = new Projectile();
	}
	else if( entityDef.m_type.compare( "Portal" ) == 0 ) {
		result = new Portal( entityDef );
	}
	else if( entityDef.m_type.compare( "Entity" ) == 0 ) {
		result = new Entity( entityDef );
	}
	return result;
}

void Map::SetDebugMode( bool isDebug )
{
	m_isDebug = isDebug;
}
