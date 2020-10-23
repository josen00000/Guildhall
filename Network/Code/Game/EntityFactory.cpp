#include "EntityFactory.hpp"
#include "Game/Entity.hpp"
#include "Game/Actor.hpp"

void EntityFactory::LoadEntityDefinitions()
{

}

Entity* EntityFactory::CreateEntityWithDefinition( std::string defName )
{
	return new Entity( EntityDefinition::s_definitions[defName] );
}

Entity* EntityFactory::SpawnNewEntityOfType( EntityType type )
{
	const EntityDefinition& entityDef = EntityDefinition::s_definitions[type];
	return SpawnNewEntityOfType( entityDef );
}

Entity* EntityFactory::SpawnNewEntityOfType( EntityDefinition const& entityDef )
{
	Entity* result = nullptr;
	if( entityDef.m_type.compare( "Actor" ) == 0 ) {
		result = new Actor( entityDef );
	}
	// 	else if( entityDef.m_type.compare( "Projectile" ) == 0 ) {
	// 		result = new Projectile();
	// 	}
	// 	else if( entityDef.m_type.compare( "Portal" ) == 0 ) {
	// 		result = new Portal( entityDef );
	// 	}
	else if( entityDef.m_type.compare( "Entity" ) == 0 ) {
		result = new Entity( entityDef );
	}
	return result;
}
