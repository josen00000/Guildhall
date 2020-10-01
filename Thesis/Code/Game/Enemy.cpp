#include "Enemy.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern Game*			g_theGame;
extern RenderContext*	g_theRenderer;

Enemy::Enemy()
{
	m_type = ACTOR_ENEMY;
	m_color = Rgba8::BLACK;
	m_isPushedByActor = false;
	m_doesPushActor = true;
}

Enemy* Enemy::SpawnPlayerWithPos( Map* map, Vec2 pos )
{
	Enemy* tempEnemy = new Enemy();
	tempEnemy->SetPosition( pos );
	tempEnemy->SetMap( map );
	tempEnemy->m_patrolGoalPos = (Vec2)( map->GetRandomInsideNotSolidTileCoords());
	return tempEnemy;
}

void Enemy::UpdateEnemy( float deltaSeconds )
{
	CheckState();
	switch( m_state )
	{
	case ENEMY_PATROL:
		// Check if goal valid
		// random find goal point
		// check if reach 
		FindGoalPatrolPoint( deltaSeconds );
		m_movingDir = m_patrolGoalPos - m_position;
		m_movingDir.Normalize();
		break;
	case ENEMY_ATTACK:
		Shoot( deltaSeconds );
		break;
	default:
		break;
	}
	__super::UpdateActor( deltaSeconds );
}

void Enemy::RenderEnemy()
{
	__super::RenderActor();
	if( g_theGame->GetIsDebug() ) {
		g_theRenderer->DrawCircle( m_patrolGoalPos, 0.1f, 0.1f, m_color );
	}
}

void Enemy::SetMap( Map* map )
{
	m_map = map;
}

void Enemy::CheckState()
{
	int playerNum = m_map->GetPlayerNum();
	for( int i = 0; i < playerNum; i++ ) {
		Vec2 tempPlayerPos = m_map->GetPlayerPosWithIndex( i );
		float distSq = GetDistanceSquared2D( tempPlayerPos, m_position );
		g_theConsole->DebugLogf( "dist sq is : %.2f", distSq );
		if( distSq < ( m_activeDistThreshold * m_activeDistThreshold ) ) {
			m_target = m_map->GetPlayerWithIndex( i );
			m_state = ENEMY_ATTACK;
			return;
		}
	}
	m_target = nullptr;
	m_state = ENEMY_PATROL;

}

void Enemy::CheckIfReachGoal()
{

}

void Enemy::FindGoalPatrolPoint( float deltaSeconds )
{
	static float totalTime = 0.f;
	totalTime += deltaSeconds;
	if( !m_map->IsPosNotSolid( m_patrolGoalPos ) ) {
		m_patrolGoalPos = (Vec2)m_map->GetRandomInsideNotSolidTileCoords();
		totalTime = 0.f;
	}
	float distSq = GetDistanceSquared2D( m_position, m_patrolGoalPos );
	while( distSq < 1 || totalTime > 5.f ) {
		m_patrolGoalPos = (Vec2)m_map->GetRandomInsideNotSolidTileCoords();
		totalTime = 0.f;
		distSq = GetDistanceSquared2D( m_position, m_patrolGoalPos );
	}
}

void Enemy::Shoot( float deltaSeconds )
{
	if( m_target == nullptr ) {
		ERROR_AND_DIE( "No player in enemy shoot!" ); 
	}
	m_movingDir = Vec2::ZERO;
	Vec2 fwdDir = m_target->GetPosition() - m_position;
	fwdDir.Normalize();
	m_orientationDegrees = fwdDir.GetAngleDegrees(); 
	static float shootTotalTime = 0.f;
	shootTotalTime += deltaSeconds;
	if( shootTotalTime > m_shootCooldown ) {
		__super::Shoot();
		shootTotalTime = 0.f;
	}
}

