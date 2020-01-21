#pragma once
#include<Game/GameCommon.hpp>
#include <Game/Entity.hpp>
class PlayerShip;
class Wasp : public Entity {
public:
	Wasp(){}
	~Wasp(){}
	Wasp(Game* iniGame,Vec2 iniPos,PlayerShip* inPlayerShip  );

public:
	void Startup();
	void Shutdown();
	virtual void Update(float deltaTime);
	const virtual void Render();
	virtual void Die();
	void CreateWasp();
	void GenerateDebris();
public:
	Vertex_PCU m_shape[SHIP_VERTEX_NUM];
	int isRotate=0;
	float m_scale=0;
	Vec2 m_acceleration=Vec2(0,0);
	PlayerShip* m_player;



};