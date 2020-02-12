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
	void CheckIfMouseIn( Vec2 mousePos );

public:
	virtual void Update(float deltaSeconds);
	virtual void Render() const;

	// Accessor
	Vec2 GetPosition()	const ;
	bool IsMouseIn()	const { return m_isMouseIn; }
	bool IsDestroyed()	const { return m_isDestroyed; }
	bool IsSelected()	const { return m_isSelected; }
	bool IsIntersect()	const { return m_isIntersect; }
	
	// Mutator
	void SetPosition( Vec2 pos );
	void SetIntersect( bool isIntersect );

	void CheckIntersectWith( GameObject* other ); 
	void DisablePhysics();
	void EnablePhysics();

public:
	bool m_isDestroyed	= false;
	bool m_isMouseIn	= false;
	bool m_isSelected	= false;
	bool m_isIntersect	= false;
	Rigidbody2D* m_rb;
};

