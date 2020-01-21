#pragma once
#include<Game/Entity.hpp>
#include<Game/GameCommon.hpp>
class Entity;
class Asteroid:public Entity{
public:
	Asteroid(){}
	~Asteroid(){}
	Asteroid(Game* iniGame, Vec2& iniPos,Vec2 iniVelocity);

public:
	void Startup();
	void Shutdown();
	virtual void Update( float deltaTime );
	const virtual void Render();
	virtual void Die();
	void CheckHealth();
	void CheckCollision();
	void CreateAsteroid();
	void GenerateDebris();
	void UpdateRotation(float deltaTime);

public:
	Vertex_PCU m_shape[ASTEROID_VERTEX_NUM];
};