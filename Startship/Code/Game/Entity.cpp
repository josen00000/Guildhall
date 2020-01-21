#include<math.h>
#include "Entity.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Entity::Entity(Game* initialGame, Vec2& initialPos)
	:m_game(initialGame)
	,m_position(initialPos)
	
{
}


void Entity::Update(float deltaTime)
{
	m_position=m_position+m_velocity*deltaTime;
	
	IsAlive();
}

const void Entity::Render()
{
	Rgba8 magenta=Rgba8(255,0,255);
	Rgba8 cyan=Rgba8(0,255,255);
	Rgba8 yellow=Rgba8(255,255,0);
	m_game->m_theRenderer->DrawCircle(m_position,m_cosmeticRadius,0.1f,magenta);
	m_game->m_theRenderer->DrawCircle(m_position,m_physicsRadius,0.1f,cyan);
	Vec2 endPoint=m_position.operator+( m_velocity);
	m_game->m_theRenderer->DrawLine(m_position,endPoint,0.2f,yellow);
}

void Entity::Die()
{
	
}

const bool Entity::IsOffscreen()
{
	if(m_position.x+m_cosmeticRadius<m_game->s_leftdown.x||m_position.x-m_cosmeticRadius>m_game->s_rightup.x||m_position.y-m_cosmeticRadius>m_game->s_rightup.y||m_position.y+m_cosmeticRadius<m_game->s_leftdown.y){
		return true;
	}
	else{
		return false;
	}
}

const Vec2 Entity::GetForwardVector()
{
	Vec2 forwardVec2= Vec2(1,0);
	forwardVec2.RotateDegrees(m_rotateDegree);
	return forwardVec2;
}

const bool Entity::IsAlive()
{
	if(m_health==0){
		m_isDead=true;
 		m_isGarbage=true;
	}
	else{
		m_isGarbage=false;
	}
	if(this->IsOffscreen()&&!m_isEnemy){
		m_isDead=true;
		m_isGarbage=true;
	}
	
	return !m_isGarbage;


}


bool Entity::IsCollisionWithScreen()
{
	if( m_position.x<m_game->s_leftdown.x+m_physicsRadius||m_position.x>m_game->s_rightup.x-m_physicsRadius||m_position.y>m_game->s_rightup.y-m_physicsRadius||m_position.y<m_game->s_leftdown.y+m_physicsRadius ) {
		return true;
	}
	else {
		return false;
	}
}
