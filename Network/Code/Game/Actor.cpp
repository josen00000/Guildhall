#include "Actor.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/vec2.hpp"

extern Game* g_theGame;

Actor::Actor( const EntityDefinition& entityDef )
	:Entity::Entity( entityDef )
{
	m_currentMoveDirt = MOVE_FRONT;
}

void Actor::Update( float deltaSeconds )
{
	m_totalTime += deltaSeconds;
	// AI
	if( m_definition->m_name == "Pinky" ){
		SetIsMoving( true );
		int totalIndex = m_totalTime;
		if( totalIndex % 2 == 0 ) {
			SetMoveDirt( Vec2( 1.f, 0.f ) );
		}
		else {
			SetMoveDirt( Vec2( -1.f, 0.f ) );
		}
	}

	Vec2 cameraPosXY = g_theGame->m_gameCamera->GetPosition().GetXYVector();
	Vec2 entityToCameraXY = cameraPosXY - m_2Dpos;
	const AnimationsSpriteDefinitions spriteDefinitions =  m_definition->GetAnimationSpriteDefinitions( m_animationType );
	float minAngle = 360.f;
	for( int i = 0; i < NUM_MOVE_DIRECTION; i++ ) {
		if( spriteDefinitions[i] == nullptr ){ continue; }
		Vec2 angleNormalLocal = m_definition->GetMoveDirtNormal( (MoveDirection) i );
		Vec2 forwardNormal = GetforwardNormal2D();
		Vec2 angleNormalWorld = Vec2( ( forwardNormal.x * angleNormalLocal.x ) + ( -forwardNormal.y * angleNormalLocal.y ), ( forwardNormal.y * angleNormalLocal.x ) + ( forwardNormal.x * angleNormalLocal.y ) );
		float tempAngle = GetAngleDegreesBetweenVectors2D( entityToCameraXY, angleNormalWorld );
		if( tempAngle < minAngle ) {
			minAngle = tempAngle;
			m_currentMoveDirt = (MoveDirection)i;
		}
	}

	SpriteAnimDefinition* currentSpriteAnimDef = spriteDefinitions[m_currentMoveDirt];
	Vec2 uvAtMin;
	Vec2 uvAtMax;
	SpriteDefinition currentSpriteDef =  currentSpriteAnimDef->GetSpriteDefAtTime( m_totalTime );
	currentSpriteDef.GetUVs( uvAtMin, uvAtMax );

	Entity::Update(deltaSeconds);

	Entity::UpdateVerts( uvAtMin, uvAtMax );
}

void Actor::UpdateVerts( float deltaSeconds )
{
	m_totalTime += deltaSeconds;
	Vec2 cameraPosXY = g_theGame->m_gameCamera->GetPosition().GetXYVector();
	Vec2 entityToCameraXY = cameraPosXY - m_2Dpos;
	const AnimationsSpriteDefinitions spriteDefinitions =  m_definition->GetAnimationSpriteDefinitions( m_animationType );
	float minAngle = 360.f;
	for( int i = 0; i < NUM_MOVE_DIRECTION; i++ ) {
		if( spriteDefinitions[i] == nullptr ) { continue; }
		Vec2 angleNormalLocal = m_definition->GetMoveDirtNormal( (MoveDirection)i );
		Vec2 forwardNormal = GetforwardNormal2D();
		Vec2 angleNormalWorld = Vec2( (forwardNormal.x * angleNormalLocal.x) + (-forwardNormal.y * angleNormalLocal.y), (forwardNormal.y * angleNormalLocal.x) + (forwardNormal.x * angleNormalLocal.y) );
		float tempAngle = GetAngleDegreesBetweenVectors2D( entityToCameraXY, angleNormalWorld );
		if( tempAngle < minAngle ) {
			minAngle = tempAngle;
			m_currentMoveDirt = (MoveDirection)i;
		}
	}

	SpriteAnimDefinition* currentSpriteAnimDef = spriteDefinitions[m_currentMoveDirt];
	Vec2 uvAtMin;
	Vec2 uvAtMax;
	SpriteDefinition currentSpriteDef =  currentSpriteAnimDef->GetSpriteDefAtTime( m_totalTime );
	currentSpriteDef.GetUVs( uvAtMin, uvAtMax );

	Entity::UpdateVerts( uvAtMin, uvAtMax );

}

