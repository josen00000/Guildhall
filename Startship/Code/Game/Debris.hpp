#pragma once
#include<Game/Entity.hpp>
#include<Game/GameCommon.hpp>
class Entity;
struct Rgba8;
class Debris:public Entity{
public:
	Debris(){}
	~Debris(){}
	Debris(Game* iniGame, Vec2& iniPos,Vec2 iniVelocity,Rgba8& iniColor);

public:
	void Startup();
	void Shutdown();
	virtual void Update( float deltaTime );
	const virtual void Render();
	void CreateDebri();
	void UpdateRotation(float deltaTime);
	void FadeoutUpdate(float deltaTime);
public:
	Vertex_PCU m_shape[DEBRIS_VERTEX_NUM];
	
	float m_fadeRate=DEBRIS_FADE_RATE;
};