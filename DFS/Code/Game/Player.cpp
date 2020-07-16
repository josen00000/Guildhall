#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern InputSystem* g_theInputSystem;
extern AudioSystem* g_theAudioSystem;
extern RenderContext* g_theRenderer;
extern Game*		g_theGame;

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
	if( !m_disableInput ) {
		HandleInput( deltaSeconds );
	}
	if( m_disableMove ) {
		m_isMoving = false;
	}
	if( m_isMoving ) {
		PlayWalkSound( deltaSeconds );
	}
	else {
// 		SoundID walkSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/WalkFoot.mp3" );
// 		g_theAudioSystem->PlaySound( walkSound, true, 1.f, 0.f, 1.f, true );
	}
	if( m_hp <= 0 ) {
		g_theGame->SetGameEnd();
	}
	if( m_isOnLava ) {
		m_hp -= 5.f * deltaSeconds;
	}

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
	UNUSED( deltaSeconds );
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

void Player::SetDisableInput( bool disableInput )
{
	m_disableInput = disableInput;
}

void Player::SetDisableMove( bool disableMove )
{
	m_disableMove = disableMove;
}

void Player::SetIsOnLava( bool onLava )
{
	m_isOnLava = onLava;
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


void Player::PlayWalkSound( float deltaSeconds )
{
	static float totalPlaySeconds = 0.f;
	static int playIndex = 0;
	float soundDuration = 0.5f;
	totalPlaySeconds += deltaSeconds;
	int playtimeIndex = (int)( totalPlaySeconds / soundDuration );
	if( playtimeIndex > playIndex ) {
		playIndex++;
		SoundID walkSound = g_theAudioSystem->CreateOrGetSound( "Data/Audio/WalkFoot.mp3" );
		g_theAudioSystem->PlaySound( walkSound, false, g_theGame->m_volume );
	}
}
