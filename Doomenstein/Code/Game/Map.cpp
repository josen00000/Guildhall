#include "Map.hpp"
#include "Game/Actor.hpp"
#include "Game/Projectile.hpp"
#include "Game/Portal.hpp"

void Map::PrepareCamera()
{

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
