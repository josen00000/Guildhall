#include "Enemy.hpp"


Enemy* Enemy::SpawnPlayerWithPos( Vec2 pos )
{
	Enemy* tempEnemy = new Enemy();
	tempEnemy->SetPosition( pos );
	return tempEnemy;
}

void Enemy::UpdateEnemy( float deltaSeconds )
{
	switch( m_state )
	{
	case ENEMY_PATROL:
		
		break;
	case ENEMY_ATTACK:
		break;
	default:
		break;
	}
}

void Enemy::RenderEnemy()
{
	__super::RenderActor();
}

void Enemy::CheckIsActive()
{
	
}

void Enemy::CheckIfReachGoal()
{

}

