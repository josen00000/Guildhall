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

Actor::Actor( Vec2 pos )
{
	m_position = pos;
}

Actor* Actor::SpawnActorWithPos( Vec2 pos )
{
	Actor* newActor = new Actor( pos );
	return newActor;
}

void Actor::UpdateActor( float deltaSeconds )
{
	Vec2 velocity = m_movingDirt * m_speed;
	m_position = m_position + velocity * deltaSeconds;
}


void Actor::RenderActor() 
{
	g_theRenderer->SetDiffuseTexture( nullptr );
	switch( m_type )
	{
	case ACTOR_NONE:
		break;
	case ACTOR_PLAYER:
		g_theRenderer->DrawCircle( Vec3( m_position, 0 ), m_physicsRadius, 0.05f, Rgba8::RED );
		break;
	case ACTOR_ENEMY:
		g_theRenderer->DrawCircle( Vec3( m_position, 0 ), m_physicsRadius, 0.05f, Rgba8::BLUE );
		break;
	default:
		break;
	}
}

void Actor::Shoot()
{
	Vec2 projectileDirt = Vec2::ONE;
	projectileDirt.SetAngleDegrees( m_orientationDegrees );
	m_map->SpawnNewProjectile( m_type, m_position, projectileDirt );
	
}

void Actor::TakeDamage( float damage )
{
	m_hp -= damage;
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
	m_movingDirt = moveDirt;
}

void Actor::SetPosition( Vec2 pos )
{
	m_position = pos;
}


