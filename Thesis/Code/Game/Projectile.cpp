#include "Projectile.hpp"
#include "Game/Actor.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;

Projectile::Projectile( ActorType type, Vec2 startPos, Vec2 movingDirt )
	:m_type( type )
	,m_pos( startPos )
	,m_movingDirection( movingDirt )
{

}

Projectile::~Projectile()
{

}


Projectile* Projectile::SpawnProjectileWithDirtAndType( Vec2 movingDirt, Vec2 startPos, ActorType type )
{
	Projectile* newProjectile = new Projectile( type, startPos, movingDirt );
	return newProjectile;
}

void Projectile::UpdateProjectile( float deltaSeconds )
{
	Vec2 velocity = m_movingDirection * m_speed;
	m_pos = m_pos + velocity * deltaSeconds;
}

void Projectile::RenderProjectile()
{
	switch( m_type )
	{
	case ACTOR_NONE:
		break;
	case ACTOR_PLAYER:
		g_theRenderer->DrawCircle( Vec3( m_pos ), m_physicsRadiu, 0.1f, Rgba8::RED );
		break;
	case ACTOR_ENEMY:
		g_theRenderer->DrawCircle( Vec3( m_pos ), m_physicsRadiu, 0.1f, Rgba8::BLUE );
		break;
	default:
		break;
	}
}
