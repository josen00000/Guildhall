#pragma once
#include "Engine/Math/vec2.hpp"

class Collider2D;
class Physics2D;
class RenderContext;
struct Rgba8;

class Rigidbody2D {
	friend class Physics2D;

public:
	Rigidbody2D( Physics2D* owner, Collider2D* col = nullptr );

private:
	~Rigidbody2D();

public:
	void Destroy();
	
	// Accessor
	Vec2 GetLocation() const { return m_worldPosition; }
	bool IsDestroied() const { return m_isDestroied; }
	Collider2D* GetCollider() const { return m_collider; }
	
	// Mutator
	void SetPosition( Vec2 position );
	void SetCollider( Collider2D* collider ); 

	// help
	void DebugRenderCollider2D( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& filledColor );

private:
	Physics2D*	m_system	= nullptr;
	Collider2D* m_collider	= nullptr;

	Vec2 m_worldPosition;
	bool m_isDestroied = false;
};