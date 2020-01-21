#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

class Game;
struct Rgba8;
class Map;

class Entity {
public:
	Entity(){}
	~Entity(){}
	Entity( Map* map, const Vec2& initialPos );

public:
	virtual void Update( float deltaSeconds );
	virtual void Render() const;
	virtual void Die();
	virtual void TakeDamage();

	//accessor
	const Vec2 GetForwardVector();
	const bool IsAlive();
	
public:
	int m_vertexNum = 0;
	float m_orientationDegrees = 0;
	float m_physicsRadius = 0;
	float m_cosmeticRadius = 0;
	float m_visionDist = 0;
	bool m_isGarbage = false;
	bool m_isDead = false;
	bool m_isPushedByWalls = false;
	bool m_isPushedByEntities = false;
	bool m_doesPushEntities = false;
	bool m_canWalk = false;
	bool m_canFly = false;
	bool m_canSwim = false;


	Rgba8 m_color;
	AABB2 m_bounds = AABB2();
	Map* m_map = nullptr;
	Vec2 m_position = Vec2(0.f,0.f);
	Vec2 m_velocity = Vec2(0.f,0.f);

protected:
	int m_health = 0;
};

typedef std::vector<Entity*> EntityList;