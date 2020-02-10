#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Physics/Collider2D.hpp"

class Game;
class Rigidbody2D;
class RandomNumberGenerator;
class Physics2D;
struct Rgba8;
struct Polygon2;

extern Physics2D* g_thePhysics;

class GameObject {
public:
	//constructor
	//GameObject( Vec2 pos, const Vec2* points, int pointCount );
	GameObject( Vec2 pos, float radius );
	GameObject( Polygon2 poly );
	GameObject( std::vector<Vec2> points );
	~GameObject();

private:
	void SetAverageCenterByPoints();

public:
	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	//accessor
	
	
public:
	bool m_isDestroyed	= false;
	bool m_isMouseIn	= false;
	bool m_isSelected	= false;
	bool m_isIntersect	= false;
	Vec2 m_position = Vec2(0.f,0.f);
	Rigidbody2D* m_rb;
};

