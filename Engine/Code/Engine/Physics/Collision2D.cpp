#include "Collision2D.hpp"

Collision2D::Collision2D( Collider2D* colA, Collider2D* colB, Manifold2D manifold )
	:me(colA)
	,other(colB)
	,manifold(manifold)
{
}

float Collision2D::GetDist() const
{
	return manifold.dist;
}


Vec2 Collision2D::GetNormal() const
{
	return manifold.normal;
}

Vec2 Collision2D::GetTangent() const
{
	Vec2 tangent = GetNormal();
	tangent.Rotate90Degrees();
	return tangent;
}

Vec2 Collision2D::GetContactCenter() const
{
	return ( manifold.contact.GetStartPos() + manifold.contact.GetEndPos() ) / 2 ;
}

Manifold2D Collision2D::GetManifold() const
{
	return manifold;
}

void Collision2D::SetContactAsVec2( Vec2 point )
{
	manifold.contact.SetEndPos( point );
	manifold.contact.SetStartPos( point );
}

void Collision2D::SetContact( LineSegment2 line )
{
	manifold.contact = line;
}

