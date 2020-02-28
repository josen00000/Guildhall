#include "Collision2D.hpp"

Collision2D::Collision2D( Collider2D* colA, Collider2D* colB, Manifold2D manifold )
	:me(colA)
	,other(colB)
	,manifold(manifold)
{
}

Vec2 Collision2D::GetNormal() const
{
	return manifold.normal;
}

Manifold2D Collision2D::GetManifold() const
{
	return manifold;
}

float Collision2D::GetDist() const
{
	return manifold.dist;
}