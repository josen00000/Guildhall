#pragma once
#include <string>
#include "Game/EntityDefinition.hpp"

class Entity;

class EntityFactory {
public:
	EntityFactory() = default;
	~EntityFactory() = default;
	EntityFactory( EntityFactory const& ) = delete;
	EntityFactory( EntityFactory const&& ) = delete;

	void LoadEntityDefinitions(); // TODO: Move from game to here
	Entity* CreateEntityWithDefinition( std::string defName );
	Entity* SpawnNewEntityOfType( EntityType type );
	Entity* SpawnNewEntityOfType( EntityDefinition const& entityDef );
};