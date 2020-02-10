#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Game;
class Rigidbody2D;
struct Rgba8;

class GameObject {
public:
	//constructor
	GameObject(){}
	~GameObject();

public:
	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	//accessor
	
public:
	bool m_isDestroyed;
	Vec2 m_position = Vec2(0.f,0.f);
	Rigidbody2D* m_rb;
};

