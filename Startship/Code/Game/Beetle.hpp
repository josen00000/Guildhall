#pragma once
#include<Game/GameCommon.hpp>
#include <Game/Entity.hpp>
class PlayerShip;
class Beetle : public Entity {
public:
	Beetle(){}
	~Beetle(){}
	Beetle(Game* iniGame,Vec2 iniPos,PlayerShip* inPlayerShip  );

public:
	void Startup();
	void Shutdown();
	virtual void Update(float deltaTime);
	const virtual void Render();
	virtual void Die();
	void CreateBeetle();
	void GenerateDebris();
	
public:
	Vertex_PCU m_shape[SHIP_VERTEX_NUM];
	float m_scale=0;
	float m_speed=BEETLE_SPEED;
	PlayerShip* m_player;



};