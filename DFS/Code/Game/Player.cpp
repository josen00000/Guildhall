#include "Game/Player.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern InputSystem* g_theInputSystem;
extern RenderContext* g_theRenderer;

Player::Player( ActorDefinition const& definition )
	:Actor::Actor(definition)
{
}

Player* Player::SpawnPlayerWithPos( ActorDefinition const& def, Vec2 pos )
{
	Player* tempPlayer = new Player( def );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds )
{
	HandleInput( deltaSeconds );
	__super::UpdateActor( deltaSeconds );
	UpdateAnimation( deltaSeconds );
}

void Player::UpdateAnimation( float deltaSeconds )
{
	static float totalSeconds = 0.f;
	totalSeconds += deltaSeconds;
	std::string moveDirt = GetMoveDirtInString( m_moveDirt );
	SpriteAnimDefinition* currentAnimDef = m_definition.m_anims[moveDirt];
	const SpriteDefinition& currentSpriteDef = currentAnimDef->GetSpriteDefAtTime( totalSeconds );
	Vec2 uvAtMin;
	Vec2 uvAtMax;
	m_texture = currentAnimDef->GetSpriteSheet().GetTexture();
	currentSpriteDef.GetUVs( uvAtMin, uvAtMax );
	__super::UpdateActorVerts( uvAtMin, uvAtMax );
}

void Player::HandleInput( float deltaSeconds )
{
	m_isMoving = true;
	if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
		m_orientationDegrees = 90.f;
		m_moveDirt = MOVE_NORTH;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
		m_orientationDegrees = -90.f;
		m_moveDirt = MOVE_SOUTH;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
		m_orientationDegrees = 180.f;
		m_moveDirt = MOVE_WEST;
	}
	else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
		m_orientationDegrees = 0.f;
		m_moveDirt = MOVE_EAST;
	}
	else {
		m_isMoving = false;
		m_moveDirt = IDLE;
	}
}

void Player::RenderPlayer()
{
	g_theRenderer->SetDiffuseTexture( m_texture );
	__super::RenderActor();
}

void Player::SetHasKey( bool hasKey )
{
	m_hasKey = hasKey;
}

std::string Player::GetMoveDirtInString( MoveDirection moveDirt ) const
{
	switch( moveDirt )
	{
	case MOVE_EAST:
		return "MoveEast";
	case MOVE_WEST:
		return "MoveWest";
	case MOVE_NORTH:
		return "MoveNorth";
	case MOVE_SOUTH:
		return "MoveSouth";
	case IDLE:
		return "Idle";
	}
	return "";
}
