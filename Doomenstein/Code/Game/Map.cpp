#include "Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"

void Map::RenderEntities() const
{
	for( int i = 0; i < m_actors.size(); i++ ) {
		m_actors[i]->Render();
	}
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
	for( int i = 0; i < m_actors.size(); i++ ) {
		m_actors[i]->Update( deltaSeconds );
	}
}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	const EntityDefinition& entityDef = EntityDefinition::s_definitions[entityDefName];
	return SpawnNewEntityOfType( entityDef );
}

Entity* Map::SpawnNewEntityOfType( EntityDefinition const& entityDef )
{
	Entity* result = nullptr;
	if( entityDef.m_type.compare( "actor" ) ){
		result = new Actor( entityDef );
	}
	else if( entityDef.m_type.compare( "Projectile" ) ) {
		result = new Projectile();
	}
	else if( entityDef.m_type.compare( "Portal" ) ) {
		result = new Portal();
	}
	else if( entityDef.m_type.compare( "Entity" ) ) {
		result = new Entity( entityDef );
	}
	return result;
}

void Map::SetDebugMode( bool isDebug )
{
	m_isDebug = isDebug;
}
