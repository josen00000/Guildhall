#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Polygon2.hpp"


class PolygonCollider2D : public Collider2D {
public:
	PolygonCollider2D(){}
	PolygonCollider2D( Polygon2 polygon );
	PolygonCollider2D( std::vector <Vec2> points  );
	~PolygonCollider2D();

public:
	virtual void UpdateWorldShape() override;

	// queries
	virtual Vec2 GetClosestPoint( const Vec2& pos ) const override;
	virtual bool Contains( const Vec2& pos ) const override;
	virtual bool Intersects( const Collider2D* other) const override;

	virtual void SetPosition( Vec2 pos ) override;

	// debug helpers
	virtual void DebugRender( RenderContext* ctx, const Rgba8& borderColor, const Rgba8& fillColor ) override;

public:
	Polygon2 m_polygon;
};