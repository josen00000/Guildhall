#include "Projectile.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"


extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;

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
	switch( type )
	{
	case ACTOR_PLAYER:
		newProjectile->m_texture = g_theGame->m_playerProjectileTexture;
		break;
	case ACTOR_ENEMY:
		newProjectile->m_texture = g_theGame->m_enemyProjectileTexture;
		break;
	}
	return newProjectile;
}

void Projectile::UpdateProjectile( float deltaSeconds )
{
	if( m_isDead ){ return; }
	Vec2 velocity = m_movingDirection * m_speed;
	m_pos = m_pos + velocity * deltaSeconds;

	m_vertices.clear();
	m_movingDirection.Normalize();
	OBB2 projectileOBB = OBB2( Vec2( 1, 1 ), m_pos, m_movingDirection );
	AppendVertsForOBB2D( m_vertices, projectileOBB, m_color );
}

void Projectile::RenderProjectile()
{
	if( m_isDead ){ return; }
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
	g_theRenderer->SetDiffuseTexture( m_texture );
	g_theRenderer->DrawVertexVector( m_vertices );
}

void Projectile::Die()
{
	m_isDead = true;
}

void Projectile::SetColor( Rgba8 color )
{
	m_color = color;
}
