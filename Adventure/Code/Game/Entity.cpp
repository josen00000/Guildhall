#include "Entity.hpp"
#include <math.h>
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;


Entity::Entity( Map* map, const Vec2& initialPos )
	:m_map(map)
	,m_position(initialPos)
{

}

void Entity::Update(float deltaTime)
{
	m_position = m_position + m_velocity*deltaTime;
	m_bounds.maxs = m_position + Vec2(0.5,0.5);
	m_bounds.mins = m_position - Vec2(0.5,0.5);
}

 void Entity::Render() const
{
	Rgba8 magenta = Rgba8( 255, 0, 255 );
	Rgba8 cyan = Rgba8( 0, 255, 255 );
	Rgba8 yellow = Rgba8( 255, 255, 0 );

	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawCircle( m_position, m_cosmeticRadius, 0.05f, magenta );
	g_theRenderer->DrawCircle( m_position, m_physicsRadius, 0.05f, cyan );
	Vec2 endPoint = m_position + m_velocity;
	g_theRenderer->DrawLine( m_position, endPoint, 0.05f, yellow );
}
void Entity::Die()
{
	m_isDead = true;		
}
void Entity::TakeDamage(){
	m_health--;
}

const Vec2 Entity::GetForwardVector()
{
	/*Vec2 forwardVec2 = Vec2(1,0);
	forwardVec2.RotateDegrees(m_rotateDegree);
	return forwardVec2;*/
	return Vec2::ZERO;
}

const bool Entity::IsAlive()
{
	return !m_isDead;
}



