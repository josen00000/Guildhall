#include "EntityFactory.hpp"
#include "Game/Entity.hpp"

void EntityFactory::LoadEntityDefinitions()
{

}

Entity* EntityFactory::CreateEntityWithDefinition( std::string defName )
{
	return new Entity( EntityDefinition::s_definitions[defName] );
}
