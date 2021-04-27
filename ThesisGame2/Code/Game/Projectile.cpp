#include "Projectile.hpp"
#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"


extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;

Projectile::Projectile( int playerIndex, Vec2 startPos, Vec2 movingDirt )
	:m_playerIndex( playerIndex )
	,m_pos( startPos )
	,m_movingDirection( movingDirt )
{

}

Projectile::~Projectile()
{

}


Projectile* Projectile::SpawnProjectileWithDirtAndType( Vec2 movingDirt, Vec2 startPos, int playerIndex )
{
	Projectile* newProjectile = new Projectile( playerIndex, startPos, movingDirt );
	newProjectile->m_texture = g_theGame->m_playerProjectileTexture;
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
	g_theRenderer->SetDiffuseTexture( m_texture );
	g_theRenderer->DrawVertexVector( m_vertices );
}

void Projectile::Die()
{
	m_isDead = true;
}

bool Projectile::IsMovingTorwards( Vec2 point ) const
{
	Vec2 disp = point - m_pos;
	float dotProduct = DotProduct2D( disp, m_movingDirection );
	return dotProduct > 0.f;
}

void Projectile::SetColor( Rgba8 color )
{
	m_color = color;
}
