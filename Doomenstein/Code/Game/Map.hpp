#pragma once
#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"

class Entity;
class EntityDefinition;

class Map {
public:
	Map(){}
	~Map(){}

	virtual void RenderMap() const = 0;
	virtual void UpdateMeshes() = 0;
	virtual void Update( float delteSeconds ) = 0;
	virtual void PrepareCamera();

	// Entity
	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName );
	virtual Entity* SpawnNewEntityOfType( EntityDefinition const& entityDef );

public:
	std::string m_name = "";
	std::vector<Entity*> m_actors;
	std::vector<Entity*> m_projectiles;
	std::vector<Entity*> m_portals;
};