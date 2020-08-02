#include "Game/Actor.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;
extern Game* g_theGame;

Actor::Actor( ActorDefinition const& definition )
{
	m_definition = definition;
	m_hp = definition.m_health;
	m_speed = definition.m_walkSpeed;
	m_physicsRadius = definition.m_physicsRadius;
	m_attackStrength = definition.m_attackStrength;
	m_shape = AABB2( Vec2( 0.f, 0.2f ), Vec2( 0.5f, 1.f ) );
}

Actor* Actor::SpawnActorWithPos( ActorDefinition const& definition, Vec2 pos )
{
	Actor* newActor = new Actor( definition );
	newActor->SetPosition( pos );
	return newActor;
}

void Actor::UpdateActor( float deltaSeconds )
{
	m_verts.clear();
	Vec2 forwardDirt = Vec2( 1.f, 0.f );
	forwardDirt.SetAngleDegrees( m_orientationDegrees );
	if( m_isMoving ) {
		m_position = m_position + forwardDirt * m_speed * deltaSeconds; 
		m_shape.SetCenter( m_position );
	}
	//AppendVertsForAABB2D( m_verts, m_shape, Rgba8::RED, Vec2::ONE, Vec2::ZERO );
}

void Actor::UpdateActorVerts( Vec2 uvAtMin, Vec2 uvAtMax )
{
	AppendVertsForAABB2D( m_verts, m_shape, Rgba8::WHITE, uvAtMin, uvAtMax );
}

void Actor::RenderActor() 
{
	g_theRenderer->DrawVertexVector( m_verts );
	if( g_theGame->GetIsDebug() ) {
		g_theRenderer->SetDiffuseTexture( nullptr );
		g_theRenderer->DrawCircle( Vec3( m_position, 0 ), m_physicsRadius, 0.05f, Rgba8::BLUE );
	}
}

void Actor::UpdateActorAnimation( float deltaSeconds )
{
	static float totalSeconds = 0.f;
	totalSeconds+= deltaSeconds;
	SpriteAnimDefinition* currentAnimDef = m_definition.m_anims["Idle"];
	const SpriteDefinition& currentSpriteDef = currentAnimDef->GetSpriteDefAtTime( totalSeconds );
	Vec2 uvAtMin;
	Vec2 uvAtMax;
	m_texture = currentAnimDef->GetSpriteSheet().GetTexture();
	currentSpriteDef.GetUVs( uvAtMin, uvAtMax );
	UpdateActorVerts( uvAtMin, uvAtMax );
}

void Actor::TakeDamage( float damage )
{
	m_hp -= damage;
}

std::string Actor::GetType() const
{
	return m_definition.m_name;
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

void Actor::SetPosition( Vec2 pos )
{
	m_position = pos;
	m_shape.SetCenter( m_position );
}

void Actor::SetShape( AABB2 const& shape )
{
	m_shape = shape;
}
