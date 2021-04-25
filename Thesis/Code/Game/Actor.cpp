#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Projectile.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;
extern Game* g_theGame;

Rgba8 Actor::s_enemyColor = Rgba8::BLUE;
Rgba8 Actor::s_playerColor = Rgba8::RED;

Actor::Actor() {
	m_baseOBBSize =  Vec2( 3.f, 3.f );
	m_barrelOBBSize = Vec2( 2.f, 2.f );
}

Actor::Actor( Vec2 pos )
{
	m_position = pos;
}

Actor* Actor::SpawnActorWithPos( Vec2 pos )
{
	Actor* newActor = new Actor( pos );
	return newActor;
}

void Actor::UpdateActor( float deltaSeconds, Rgba8 color /*= Rgba8::WHITE */ )
{
	m_shootTimer += deltaSeconds;

	Vec2 velocity = m_movingDir * m_speed;
	m_position = m_position + velocity * deltaSeconds;
	if( m_movingDir != Vec2::ZERO ) {
		m_lastFrameMovingDegrees = m_movingDir.GetAngleDegrees();
	}

	OBB2 baseOBB = OBB2( m_baseOBBSize, m_position, m_lastFrameMovingDegrees );
	OBB2 barrelOBB = OBB2( m_barrelOBBSize, m_position, m_orientationDegrees );
	m_baseVertices.clear();
	m_barrelVertices.clear();
	AppendVertsForOBB2D( m_baseVertices, baseOBB, color );
	AppendVertsForOBB2D( m_barrelVertices, barrelOBB, color );
}

void Actor::RenderActor() 
{
	Texture* baseTexture = nullptr;
	Texture* barrelTexture = nullptr;
	switch( m_type )
	{
	case ACTOR_NONE:
		break;
	case ACTOR_PLAYER:
		baseTexture = g_theGame->m_playerTexture;
		barrelTexture = g_theGame->m_playerBarrelTexture;
		//g_theRenderer->DrawCircle( Vec3( m_position, 0 ), m_physicsRadius, 0.05f, Rgba8::RED );
		break;
	case ACTOR_ENEMY:
		baseTexture = g_theGame->m_enemyTexture;
		barrelTexture = g_theGame->m_enemyBarrelTexture;
		//g_theRenderer->DrawCircle( Vec3( m_position, 0 ), m_physicsRadius, 0.05f, Rgba8::BLUE );
		break;
	default:
		break;
	}
	g_theRenderer->SetDiffuseTexture( baseTexture );
	g_theRenderer->DrawVertexVector( m_baseVertices );
	g_theRenderer->SetDiffuseTexture( barrelTexture );
	g_theRenderer->DrawVertexVector( m_barrelVertices );
}

void Actor::Shoot( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	if( m_shootTimer < m_shootCoolDown ){ return; }

	m_shootTimer = 0.f;
	Vec2 projectileDirt = Vec2::ONE;
	projectileDirt.SetAngleDegrees( m_orientationDegrees );
	m_map->SpawnNewProjectile( m_type, m_position, projectileDirt, m_color);
}

void Actor::TakeDamage( float damage )
{
	m_hp -= damage;
	if( m_hp <= 0 ) {
		Die();
	}
}

void Actor::Die()
{
	m_aliveState = WAIT_FOR_DELETE;
}

Vec2 Actor::GetHeadDir() const
{
	Vec2 headDir = Vec2::ONE;
	headDir.SetAngleDegrees( m_orientationDegrees );
	return headDir;
}

void Actor::SetOrientationDegrees( float orientationDegrees )
{
	m_orientationDegrees = orientationDegrees;
}

void Actor::SetSpeed( float speed )
{
	m_speed = speed;
}

void Actor::SetPhysicsRadius( float physicsRadius )
{
	m_physicsRadius = physicsRadius;
}

void Actor::SetHealth( float hp )
{
	m_hp = hp;
}

void Actor::SetAttackStrength( float attackStrength )
{
	m_attackStrength = attackStrength;
}

void Actor::SetMoveDirt( Vec2 moveDirt )
{
	m_movingDir = moveDirt;
}

void Actor::SetPosition( Vec2 pos )
{
	m_position = pos;
}

void Actor::SetBaseOBBSize( Vec2 size )
{
	m_baseOBBSize = size;
}

void Actor::SetBarrelOBBSize( Vec2 size )
{
	m_barrelOBBSize = size;
}

void Actor::SetAliveState( AliveState state )
{
	m_aliveState = state;
}

