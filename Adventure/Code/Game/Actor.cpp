#include "Actor.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/World.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;
extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern Game* g_theGame;

Actor::Actor( Map* map, Vec2 spawnPosition, ActorDefinition* actorDef )
	:Entity(map, spawnPosition )
{
	m_definition = actorDef;
	m_name = m_definition->m_name;
	Vec2 min = m_position - Vec2(0.5,0.5);
	Vec2 max = m_position + Vec2(0.5,0.5);
	m_shape.mins = min;
	m_shape.maxs = max;

}

void Actor::Update( float deltaSeconds )
{
	//UpdateByXboxInput(deltaSeconds);
	m_velocity = Vec2(-0.5,0); 
	Entity::Update( deltaSeconds );
	// Update shape and vertices

}

 void Actor::Render()const
{
	if(g_theGame->m_developMode){
		Entity::Render();
	}
	Texture* actorTexture = g_theRenderer->CreateOrGetTextureFromFile( m_definition->m_spriteFilePath.c_str() );
	g_theRenderer->BindTexture( actorTexture );
	const std::vector<Vertex_PCU> actorVerticesInWorld = TransformVertexArray( m_vertices, 1, m_orientationDegrees, m_position );
	g_theRenderer->DrawVertexVector( actorVerticesInWorld );
}

void Actor::Die()
{
	Entity::Die();
}

void Actor::TakeDamage()
{
	Entity::TakeDamage();
	if( m_health <= 0 ) {
		Die();
	}
}

void Actor::UpdateByXboxInput( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	const XboxController xboxController = g_theInputSystem->GetXboxController(0);

	float magnitude = 0.f;
	float turningDegrees = 0.f;
	if(xboxController.isConnected()){
		AnalogJoystick leftJoystick =xboxController.GetLeftJoystick();
		magnitude = leftJoystick.GetMagnitude();
		turningDegrees = leftJoystick.GetAngleDegrees();
		Vec2 moveDirection = Vec2::ONE;
		moveDirection.SetAngleDegrees(turningDegrees);
		m_velocity = moveDirection * magnitude;
	}
}

void Actor::CreateActorShapes()
{
	AABB2 shape = m_definition->m_localDrawBound;
	m_vertices.push_back( Vertex_PCU( Vec3( shape.mins ), Rgba8( 255, 255, 255 ), Vec2::ZERO ));
	m_vertices.push_back( Vertex_PCU( Vec3( shape.maxs ), Rgba8( 255, 255, 255 ), Vec2::ONE ));
	m_vertices.push_back( Vertex_PCU( Vec3( shape.mins.x, shape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2(0,1) ));

	m_vertices.push_back( Vertex_PCU( Vec3( shape.mins ), Rgba8( 255, 255, 255 ), Vec2::ZERO ) );
	m_vertices.push_back( Vertex_PCU( Vec3( shape.maxs ), Rgba8( 255, 255, 255 ), Vec2::ONE ) );
	m_vertices.push_back( Vertex_PCU( Vec3( shape.maxs.x, shape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
}

