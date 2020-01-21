#pragma once
#include<Engine/Core/EngineCommon.hpp>
class Game;
struct Rgba8;
class Entity {
public:
	//constructor
	Entity(){}
	~Entity(){}
	Entity(Game* initialGame, Vec2& initialPos );
public:
	virtual void Update(float deltaTime);
	const virtual void Render();
	virtual void Die();

public:
	//accessor
	const bool IsOffscreen();
	const Vec2 GetForwardVector();
	const bool IsAlive();
	
	//void GeneratePhysicCircle();
	//void GenerateCosmeticCircle();
	bool IsCollisionWithScreen();
public:
	Vec2 m_position= Vec2(0.f,0.f);
	Vec2 m_velocity= Vec2(0.f,0.f);
	float m_orientationDegrees=0;
	float m_angularVelocity=0;
	float m_physicsRadius=0;
	float m_cosmeticRadius=0;
	float m_rotateDegree=0;
	int m_health=0;
	int m_collisionState=0;
	int m_vertexNum=0;
	int m_debrisNum=0;
	bool m_isOffscreen=false;
	bool m_isGarbage=false;
	bool m_isDead=false;
	bool ableDebri=true;
	bool m_isEnemy=false;
	Game* m_game=nullptr;
	Rgba8 m_color;

	//Vertex_PCU* physicCircle[64];



};