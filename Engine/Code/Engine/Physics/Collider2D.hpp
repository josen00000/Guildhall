#pragma once

class Physics2D;
class RenderContext;
class Rigidbody2D;

struct Rgba8;
struct Vec2;
	
enum Collider2DType {
	COLLIDER2D_DISC,
};

class Collider2D {
	friend class Physics2D;

public:
	virtual void UpdateWorldShape() = 0;

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos )		const = 0;
	virtual bool Contains( const Vec2& pos )			const = 0;
	virtual bool Intersects( const Collider2D* other )	const = 0;

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) = 0;

protected:
	virtual ~Collider2D() = 0;

private:
	Collider2DType	m_type;
	Physics2D*		m_system	= nullptr;
	Rigidbody2D*	m_rigidbod	= nullptr;
};