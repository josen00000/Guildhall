#pragma once
#include <string>

class Entity;

class EntityFactory {
public:
	EntityFactory() = default;
	~EntityFactory() = default;
	EntityFactory( EntityFactory const& ) = delete;
	EntityFactory( EntityFactory const&& ) = delete;

	void LoadEntityDefinitions(); // TODO: Move from game to here
	Entity* CreateEntityWithDefinition( std::string defName );

};