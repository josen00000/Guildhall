#pragma once
#include "Engine/Core/EngineCommon.hpp"


class Game;
struct Rgba8;
class Map;

enum EntityFaction {
	ENTITY_FACTION_NULL = -1,
	ENTITY_FACTION_GOOD,
	ENTITY_FACTION_NEUTRAL,
	ENTITY_FACTION_EVIL,
	ENTITY_NUM_FACTION
};

enum EntityType {
	ENTITY_TYPE_NULL = -1,
	ENTITY_TYPE_PLAYER,
	ENTITY_TYPE_NPC_TURRET,
	ENTITY_TYPE_NPC_TANK, 
	ENTITY_TYPE_BOULDER, 
	ENTITY_TYPE_BULLET, 
	ENTITY_TYPE_EXPLOSION, 
	NUM_ENTITY_TYPES
};


class Entity {
public:
	//constructor
	Entity(){}
	~Entity(){}
	Entity(Map* map, const Vec2& initialPos ,EntityType entityType, EntityFaction entityFaction );

public:
	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	virtual void Die();
	virtual void TakeDamage();

	//accessor
	const Vec2 GetForwardVector();
	const bool IsAlive();
	
public:
	Map* m_map = nullptr;
	Vec2 m_position = Vec2(0.f,0.f);
	Vec2 m_velocity = Vec2(0.f,0.f);
	float m_orientationDegrees = 0;
	float m_angularVelocity = 0;
	float m_physicsRadius = 0;
	float m_cosmeticRadius = 0;
	float m_rotateDegree = 0;
	int m_vertexNum = 0;
	bool m_isGarbage = false;
	bool m_isDead = false;
	Rgba8 m_color;
	EntityType m_type = ENTITY_TYPE_NULL;
	EntityFaction m_faction = ENTITY_FACTION_NULL;
	bool m_isPushedByWalls = false;
	bool m_isPushedByEntities = false;
	bool m_doesPushEntities = false;
	bool m_isHitByBullet = false;
	float m_visionDist =0;
protected:
	int m_health = 0;
};

typedef std::vector< Entity* > EntityList;