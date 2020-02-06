#include "Collider2D.hpp"
#include "Engine/Core/EngineCommon.hpp"

void Collider2D::Destroy()
{
	m_isDestroied = true;
}

void Collider2D::SetPosition( Vec2 pos )
{
	UNUSED(pos);
}

Collider2D::~Collider2D()
{

}
