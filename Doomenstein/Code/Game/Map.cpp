#include "Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"

void Map::PrepareCamera()
{

}

MapRaycastResult Map::RayCast( Vec3 startPos, Vec3 endPos )
{
	Vec3 raycastDisp = endPos - startPos;
	Vec3 forwardNormal = raycastDisp.GetNormalized();
	float maxdist = raycastDisp.GetLength();

	return RayCast( startPos, forwardNormal, maxdist );
}

Entity* Map::SpawnNewEntityOfType( std::string const& entityDefName )
{
	Entity* spawnedEntity;
	if( entityDefName.compare( "Actor" ) == 0 ){ }
	else if( entityDefName.compare( "Projectile" ) == 0 ){ }
	else if( entityDefName.compare( "Portal" ) == 0 ){ }

	return nullptr;
}

Entity* Map::SpawnNewEntityOfType( EntityDefinition const& entityDef )
{
	//Entity*
	return nullptr;
}

void Map::SetDebugMode( bool isDebug )
{
	m_isDebug = isDebug;
}
