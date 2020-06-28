#pragma once
#include <vector>
#include <string>
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

class Entity;
class EntityDefinition;


struct MapRaycastResult {
	Vec3 startPosition			= Vec3::ZERO;
	Vec3 forwardNormal			= Vec3::ZERO;
	float maxDistance			= 0.f;
	bool didImpact				= false;
	Vec3 impactPosition			= Vec3::ZERO;
	Entity* impactEntity		= nullptr;
	float impactDistance		= 0.f;
	Vec3 impactSurfaceNormal	= Vec3::ZERO;
};

class Map {
public:
	Map(){}
	~Map(){}

	virtual void RenderMap() const = 0;
	virtual void UpdateMeshes() = 0;
	virtual void Update( float delteSeconds ) = 0;
	virtual void PrepareCamera();
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 forwardNormal, float maxDistance ) = 0;
	virtual MapRaycastResult RayCast( Vec3 startPos, Vec3 endPos );

	// Entity
	virtual Entity* SpawnNewEntityOfType( std::string const& entityDefName );
	virtual Entity* SpawnNewEntityOfType( EntityDefinition const& entityDef );

	void SetDebugMode( bool isDebug );
public:
	bool m_isDebug = false;
	std::string m_name = "";
	std::vector<Entity*> m_actors;
	std::vector<Entity*> m_projectiles;
	std::vector<Entity*> m_portals;
};