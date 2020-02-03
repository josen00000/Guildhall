#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"


class DiscCollider2D : public Collider2D {
public:
	DiscCollider2D();
	explicit DiscCollider2D( const Vec2& localPos, const Vec2& worldPos, float radius );
	~DiscCollider2D();

public:
	virtual void UpdateWorldShape() override;

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos ) const override;
	virtual bool Contains( const Vec2& pos ) const override;
	virtual bool Intersects( const Collider2D* other) const override;

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) override;

private:
	Vec2 m_localPosition	= Vec2::ZERO;
	Vec2 m_worldPosition	= Vec2::ZERO;
	float m_radius			= 0;
};