#pragma once

class Physics2D;
class RenderContext;
class Rigidbody2D;

struct Rgba8;
struct Vec2;
	
enum Collider2DType {
	COLLIDER2D_DISC
};

class Collider2D {
	friend class Physics2D;

protected:
	virtual ~Collider2D();

public:
	virtual void UpdateWorldShape() = 0;
	virtual void Destroy();

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos )		const = 0;
	virtual bool Contains( const Vec2& pos )			const = 0;
	virtual bool Intersects( const Collider2D* other )	const = 0;

	// Accessor
	virtual bool IsDestroied() const { return m_isDestroied; }

	// Mutator

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) = 0;


private:
	bool			m_isDestroied = false;
	Collider2DType	m_type;
	Physics2D*		m_system	= nullptr;
	Rigidbody2D*	m_rigidbody	= nullptr;
};