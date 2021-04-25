#include "Game/Item.hpp"
#include "Engine/Core/Rgba8.hpp"	
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

Item::Item( Vec2 spawnPos )
{
	m_pos = spawnPos;
	m_shape = AABB2( Vec2::ZERO, Vec2( 1.f, 1.f ) );
	m_shape.SetCenter( spawnPos );
}

void Item::SetPosition( Vec2 pos )
{
	m_pos = pos;
}

void Item::RenderItem()
{
	g_theRenderer->DrawAABB2D( m_shape, Rgba8::WHITE );
}

