#include "Entity.hpp"
#include <math.h>
#include "Game/Game.hpp"
#include "Game/App.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;




Entity::Entity( const Vec2& initialPos, EntityType entityType, EntityFaction entityFaction )
	:m_position(initialPos)	
	,m_type(entityType)
	,m_faction(entityFaction)
{

}

void Entity::Update(float deltaTime)
{
	m_position = m_position + m_velocity*deltaTime;
}

 void Entity::Render() const
{
	Rgba8 magenta = Rgba8( 255, 0, 255 );
	Rgba8 cyan = Rgba8( 0, 255, 255 );
	Rgba8 yellow = Rgba8( 255, 255, 0 );
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->DrawCircle( m_position, m_cosmeticRadius, 0.05f, magenta );
	g_theRenderer->DrawCircle( m_position, m_physicsRadius, 0.05f, cyan );
	Vec2 endPoint = m_position.operator+( m_velocity );
	g_theRenderer->DrawLine( m_position, endPoint, 0.05f, yellow );
}
void Entity::Die()
{
	m_isDead = true;		
}
void Entity::TakeDamage(){

}
const Vec2 Entity::GetForwardVector()
{
	Vec2 forwardVec2 = Vec2(1,0);
	forwardVec2.RotateDegrees(m_rotateDegree);
	return forwardVec2;
}

const bool Entity::IsAlive()
{
	return m_isDead;
}



