#include "Player.hpp"
#include "Game/Enemy.hpp"
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

Player* Player::SpawnPlayerWithPos( Vec2 pos )
{
	Player* tempPlayer = new Player();
	tempPlayer->SetSpeed( 0.f );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds )
{
	CheckInputMethod();
	m_movingDir = Vec2::ZERO;
	
	m_disableInputSeconds -= deltaSeconds;
	if( m_disableInputSeconds < 0.f ) {
		m_isAbleInput = true;
	}

	if( !g_theConsole->IsOpen()) {
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
	if( !m_isAbleInput ){ return; }
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
			__super::Shoot( deltaSeconds );
		}
		break;
	case AI_INPUT:
		CheckAIState();
		switch( m_aiState )
		{
		case PLAYER_PATROL:
			FindGoalPatrolPoint( deltaSeconds );
			m_movingDir = m_patrolGoalPos - m_position;
			m_movingDir.Normalize();
			break;
		case PLAYER_ATTACK:
			AIShoot( deltaSeconds );
			break;
		default:
			break;
		}
		break;
	case CONTROLLER_INPUT:
		const XboxController* xboxController = g_theInputSystem->GetXboxController( m_playerIndex - 1 );
		if( !xboxController->isConnected() ) { return; }


		AnalogJoystick leftJoyStick = xboxController->GetLeftJoystick();
		{
			float magnitude = leftJoyStick.GetMagnitude();
			m_movingDir = Vec2::ZERO;
			if( magnitude > 0 ) {
				m_movingDir = Vec2( magnitude, 0.f );
				m_movingDir.SetAngleDegrees( leftJoyStick.GetAngleDegrees() );
			}
		}
		
		AnalogJoystick rightJoyStick = xboxController->GetRightJoystick();
		{
			float magnitude = rightJoyStick.GetMagnitude();
			if( magnitude > 0 ) {
				m_orientationDegrees = rightJoyStick.GetAngleDegrees();
				m_orientationDegrees = ClampDegressTo360( m_orientationDegrees );
			}
		}

		if( xboxController->GetRightTrigger() > 0 ) {
			__super::Shoot( deltaSeconds );
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

bool Player::IsControlledByUser() const
{
	return ( !( m_inputState == AI_INPUT ) );
}

void Player::SetMap( Map* map )
{
	m_map = map;
}

void Player::SetPlayerIndex( int index )
{
	m_playerIndex = index;
}

void Player::SetInputControlState( InputControlState state )
{
	m_inputState = state;
}

void Player::DisableInputForSeconds( int seconds )
{
	m_disableInputSeconds = seconds;
}

void Player::CheckInputMethod()
{
	if( m_playerIndex == 0 ) {
		m_inputState = KEYBOARD_INPUT;
	}
	else {
		XboxController const* controller = g_theInputSystem->GetXboxController( m_playerIndex - 1 );
		if( !controller->isConnected() ) {
			m_inputState = AI_INPUT;
		}
		else {
			m_inputState = CONTROLLER_INPUT;
		}
	}
}

void Player::CheckAIState()
{
	std::vector<Enemy*> enemies = m_map->GetEnemies();
	for( int i = 0; i < enemies.size(); i++ ) {
		Enemy* tempEnemy = enemies[i];
		if( tempEnemy == nullptr ){ continue; }

		Vec2 tempEnemyPos = tempEnemy->GetPosition();
		float distSq = GetDistanceSquared2D( tempEnemyPos, m_position );
		if( distSq < (m_activeDistThreshold * m_activeDistThreshold) ) {
			m_target = tempEnemy;
			m_aiState = PLAYER_ATTACK;
			return;
		}
	}
	m_target = nullptr;
	m_aiState = PLAYER_PATROL;
}

void Player::FindGoalPatrolPoint( float deltaSeconds )
{
	m_patrolTotalTime += deltaSeconds;
	float distSq = GetDistanceSquared2D( m_position, m_patrolGoalPos );

	if( distSq < 0.7f || m_patrolTotalTime > 5.f || !m_map->IsPosNotSolid( m_patrolGoalPos ) ) {
		m_patrolTotalTime = 0.f;
		m_patrolGoalPos = (Vec2)m_map->GetRandomInsideNotSolidTileCoords();
	}
}

void Player::AIShoot( float deltaSeconds )
{
	if( m_target == nullptr ) {
		ERROR_AND_DIE( "No enemy in player shoot!" );
	}
	m_movingDir = Vec2::ZERO;
	Vec2 fwdDir = m_target->GetPosition() - m_position;
	fwdDir.Normalize();
	m_orientationDegrees = fwdDir.GetAngleDegrees();

	__super::Shoot( deltaSeconds );
}

