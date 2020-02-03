#pragma once
#include "Engine/Math/vec2.hpp"

class Collider2D;
class Physics2D;


class Rigidbody2D {
	friend class Physics2D;

public:
	Rigidbody2D( Physics2D* owner, Collider2D* col = nullptr );

private:
	~Rigidbody2D();

public:
	void Destroy();
	void TakeCollider( Collider2D* collider ); 
	
	// Accessor
	Vec2 GetLocation() const { return m_worldPosition; }
	bool IsDestroied() const {return m_isDestroied; }
	
	// Mutator
	void SetPosition( Vec2 position );

private:
	Physics2D*	m_system	= nullptr;
	Collider2D* m_collider	= nullptr;

	Vec2 m_worldPosition;
	bool m_isDestroied = true;
};