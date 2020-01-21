#include "Explosion.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Vec3.hpp"

extern RenderContext* g_theRenderer;

Explosion::Explosion( Map* map, Vec2 spawnPos)
	:Entity(map,spawnPos,ENTITY_TYPE_EXPLOSION, ENTITY_FACTION_NEUTRAL)
{	
	
	
}





void Explosion::Update( float deltaSeconds )
{
	m_currentTime += deltaSeconds;
	m_explosionVertices.clear();
	const SpriteDefinition& explosionSpriteDef = m_animation->GetSpriteDefAtTime(m_currentTime);
	Vec2 minUV;
	Vec2 maxUV;
	explosionSpriteDef.GetUVs(minUV, maxUV);
	Vertex_PCU leftDown = Vertex_PCU(Vec3(-0.5, -0.5, 0),Rgba8::WHITE, minUV);
	Vertex_PCU leftUp = Vertex_PCU(Vec3(-0.5, 0.5, 0),Rgba8::WHITE, Vec2(minUV.x, maxUV.y));
	Vertex_PCU rightDown = Vertex_PCU(Vec3(0.5, -0.5, 0),Rgba8::WHITE, Vec2(maxUV.x, minUV.y));
	Vertex_PCU rightUp = Vertex_PCU(Vec3(0.5, 0.5, 0),Rgba8::WHITE, maxUV);

	//Triangle A
	m_explosionVertices.push_back(leftDown);
	m_explosionVertices.push_back(rightDown);
	m_explosionVertices.push_back(leftUp);
	//Triangle B
	m_explosionVertices.push_back(rightDown);
	m_explosionVertices.push_back(leftUp);
	m_explosionVertices.push_back(rightUp);

	if(m_currentTime > m_duration){
		Die();
		if(m_isInBrick){
			m_map->SetBrickTileToDeault(m_position);
		}
	}
}

void Explosion::Render() const
{
	const std::vector<Vertex_PCU> explosionVerticesInWorld = TransformVertexArray(m_explosionVertices,m_radiu,m_orientationDegrees, m_position );
	g_theRenderer->BindTexture(m_texture);
	g_theRenderer->DrawVertexVector(explosionVerticesInWorld);
}

void Explosion::Die()
{
	Entity::Die();
}

void Explosion::CreateSpriteAnimation()
{
	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Explosion_5x5.png");
	m_explosionSprite = new SpriteSheet(*m_texture, IntVec2(5,5));
	m_animation = new SpriteAnimDefinition(*m_explosionSprite, m_startSpriteIndex, m_endSpriteIndex, m_frameDuration, SpriteAnimPlaybackType::ONCE);
	m_frameDuration = m_duration / (m_endSpriteIndex - m_startSpriteIndex + 1);
	m_orientationDegrees = m_rng.GetRandomFloatInRange( 0, 360 );
}



