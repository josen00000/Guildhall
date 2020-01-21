#include "Entity.hpp"
#include <Game/Game.hpp>
#include<math.h>
Entity::Entity(Game* initialGame, Vec2& initialPos)
	:m_game(initialGame)
	,m_position(initialPos)
	
{
}


void Entity::Update(float deltaTime)
{
	m_position=m_position+m_velocity*deltaTime;
}

const void Entity::Render()
{
}
void Entity::Die()
{
	m_isDead=true;		
}
void Entity::TakeDamage(){

}
const Vec2 Entity::GetForwardVector()
{
	Vec2 forwardVec2= Vec2(1,0);
	forwardVec2.RotateDegrees(m_rotateDegree);
	return forwardVec2;
}

const bool Entity::IsAlive()
{
	return m_isDead;
}



