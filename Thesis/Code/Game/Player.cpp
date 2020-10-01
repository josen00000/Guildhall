#include "Player.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;

Player::Player()
{
	m_type = ACTOR_PLAYER;
	m_color = Rgba8::RED;
	m_isPushedByActor = true;
	m_doesPushActor = true;
}

Player::Player( int index )
{
	m_playerIndex = index;
}

Player* Player::SpawnPlayerWithPos( Vec2 pos )
{
	Player* tempPlayer = new Player();
	tempPlayer->SetSpeed( 0.f );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds )
{
	if( !g_theConsole->IsOpen() ) {
		HandleInput( deltaSeconds );
	}

	UpdatePlayerSpeed( deltaSeconds );
	__super::UpdateActor( deltaSeconds );
}

void Player::UpdatePlayerSpeed( float deltaSeconds )
{
	m_isMoving = false;
	m_isContinousWalk = false;

	if( !IsVec2MostlyEqual( m_movingDir, Vec2::ZERO ) ) {
		m_isMoving = true;
	}

	if( m_isMoving ) {
		m_speed += m_accelerate * deltaSeconds;
		if( m_speed > 1.5f ) {
			m_isContinousWalk = true;
		}
		m_speed = ClampFloat( 0.f, m_maxSpeed, m_speed );
	}
	else {
		m_speed = 0.f;
	}
}

void Player::HandleInput( float deltaSeconds )
{
	m_movingDir = Vec2::ZERO;
	switch( m_inputState )
	{
	case KEYBOARD_INPUT:
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_W ) ) {
			m_movingDir.y = 1.f;
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_S ) ) {
			m_movingDir.y = -1.f;
		}
		
		if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_D ) ) {
			m_movingDir.x = 1.f;
		}
		else if( g_theInputSystem->IsKeyDown( KEYBOARD_BUTTON_ID_A ) ) {
			m_movingDir.x = -1.f;
		}
		if( m_movingDir.GetLength() != 0 ) {
			m_movingDir.Normalize();
		}
		if( g_theInputSystem->WasMouseButtonJustPressed( MOUSE_BUTTON_LEFT ) ) {
			Shoot();
		}
		break;
	case CONTROLLER_INPUT:
		const XboxController* xboxController = g_theInputSystem->GetXboxController( m_playerIndex );

		if( xboxController->isConnected() ) {

		}
		break;
	}
}

void Player::RenderPlayer()
{
	__super::RenderActor();
	Vec2 forwardDirt = Vec2::ONE;
	forwardDirt.SetAngleDegrees( m_orientationDegrees );
	g_theRenderer->DrawLine( m_position, m_position + forwardDirt * 1.f, 0.1f, m_color );
}

void Player::SetMap( Map* map )
{
	m_map = map;
}

void Player::SetInputControlState( InputControlState state )
{
	m_inputState = state;
}

