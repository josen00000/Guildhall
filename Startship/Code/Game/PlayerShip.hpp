#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

class XboxController;

class PlayerShip : public Entity {
public:
	PlayerShip(){}
	~PlayerShip(){}
	PlayerShip(Game* iniGame,Vec2 iniPos,  const XboxController* iniXboxController);

public:
	void Startup();
	void Shutdown();
	virtual void Update(float deltaTime);
	const virtual void Render();
	virtual void Die();
	void CreateShip();
	void CheckCollision();
	void BounceAway();
	void UpdateSTRA(float deltaTime,int rotateDirection);
	void UpdateFromXBox(float deltaTime);
	void GenerateDebris();
	void Reset(Vec2 resetPos);
	void UpdateVibration(float deltaTime);
public:
	Vertex_PCU m_shape[SHIP_VERTEX_NUM];
	int isRotate=0;
	bool isAccelerate=false;
	float m_scale=0;
	float m_accelerate=PLAYER_SHIP_ACCELERATION;
	const XboxController* m_xboxController;
	float m_vibrationIntense=0;
	float m_vibrationDownRate=0.5;
	



};