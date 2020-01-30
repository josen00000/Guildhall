#pragma once
#include "Engine/Math/vec2.hpp"

class Collider2D;
class Physics2D;


class Rigidbody2D {
	friend class Physics2D;

public:
	void Destroy();
	void TakeCollider( Collider2D* collider ); 
	void SetPosition( Vec2 position );

public:
	Physics2D*	m_system	= nullptr;
	Collider2D* m_collider	= nullptr;

	Vec2 m_worldPosition;

private:
	~Rigidbody2D();
};