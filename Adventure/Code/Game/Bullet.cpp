#include "Bullet.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Texture.hpp"

extern RenderContext* g_theRenderer;



Bullet::Bullet( Map* map, Vec2 iniPos,  EntityType entityType, EntityFaction entityFaction, Vec2 fwdDir )
	:Entity(map, iniPos, entityType, entityFaction)
	,m_fwdDir(fwdDir)
{
	CreateBullet();
}

Bullet::~Bullet()
{
	delete m_bulletTexture;
	m_bulletTexture = nullptr;
}

void Bullet::Update( float deltaSeconds )
{
	if(m_isInWater){
		m_velocity = m_fwdDir * BULLET_WATER_SPEED;
	}
	else {
		m_velocity = m_fwdDir * BULLET_SPEED;
	}
	m_orientationDegrees = m_fwdDir.GetAngleDegrees();
	Entity::Update(deltaSeconds);
}

void Bullet::Render() const
{
	const std::vector<Vertex_PCU> m_bulletShapeInWorld=TransformVertexArray(m_bulletVertices,1,m_orientationDegrees,m_position);
	g_theRenderer->BindTexture( m_bulletTexture );
	g_theRenderer->DrawVertexVector( m_bulletShapeInWorld );

}

void Bullet::Die()
{
	m_map->SpawnNewExplosion(m_position, BULLET_EXPLOSION_RADIU, BULLET_EXPLOSION_DURATION);
	Entity::Die();
}

void Bullet::TakeDamage()
{

}

void Bullet::CreateBullet()
{
	m_bulletShape = AABB2(Vec2(-.1f,-.1f),Vec2(.1f,.1f));

	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.mins.x, m_bulletShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.maxs.x, m_bulletShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
	m_bulletVertices.push_back( Vertex_PCU( Vec3( m_bulletShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

	m_bulletTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Bullet.png");
}
