#include "Player.hpp"
#include "Game/Map/Map.hpp"
#include "../../Thesis/Code/Game/Camera/CameraSystem.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/EventSystem.hpp"

extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;
extern DevConsole*		g_theConsole;
extern CameraSystem*	g_theCameraSystem;
extern EventSystem*		g_theEventSystem;

Player::Player( int index )
{
	m_type = ACTOR_PLAYER;
	m_isPushedByActor = true;
	m_doesPushActor = true;
	m_index = index;
	index++;
	unsigned char alpha = 255;
	unsigned char intensity = 200;
	if( m_index == 0 ) {
		m_color = Rgba8( intensity, intensity, 0, alpha );
	}
	else if( m_index == 1 ) {
		m_color = Rgba8( 0, intensity, intensity, alpha );
	}
	else if( m_index == 2 ) {
		m_color = Rgba8( intensity, 0, intensity, alpha );
	}
	else if( m_index == 3 ) {
		m_color = Rgba8::WHITE;
	}
}

Player* Player::SpawnPlayerWithPos( Vec2 pos, int index )
{
	Player* tempPlayer = new Player( index );
	tempPlayer->SetSpeed( 0.f );
	tempPlayer->SetPosition( pos );
	return tempPlayer;
}

void Player::UpdatePlayer( float deltaSeconds, int playerIndex )
{
	if( m_aliveState != AliveState::ALIVE ){ return; }
	m_movingDir = Vec2::ZERO;
	
	m_disableInputSeconds -= deltaSeconds;
	if( m_disableInputSeconds < 0.f ) {
		m_isAbleInput = true;
	}

	if( !g_theConsole->IsOpen()) {
		HandleInput( deltaSeconds, playerIndex );
	}

	UpdatePlayerSpeed( deltaSeconds );
	if( g_theCameraSystem->GetSplitScreenState() == VORONOI_SPLIT ) {
		UpdateMarkers();
		CameraController* controller = g_theCameraSystem->GetCameraControllerWithIndex( m_index );
		float incircleRadius = controller->GetVoronoiInCircleRadius();
		if( incircleRadius > 4.5f ) {
			controller->SetAimFocusDist( 1.5f );
		}
		else {
			controller->SetAimFocusDist( incircleRadius / 3.f );

		}
	}
	__super::UpdateActor( deltaSeconds, m_color );
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

void Player::HandleInput( float deltaSeconds, int playerIndex )
{
	UNUSED( deltaSeconds );
	if( !m_isAbleInput ){ return; }
	const XboxController* xboxController = g_theInputSystem->GetXboxController( playerIndex );
	if( !xboxController ){ return; }
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
		if( m_shootTimer < m_shootCoolDown ) { return; }

		m_shootTimer = 0.f;
		Vec2 projectileDirt = Vec2::ONE;
		projectileDirt.SetAngleDegrees( m_orientationDegrees );
		m_map->SpawnNewProjectile( m_index, m_position, projectileDirt, m_color );
	}
}

void Player::RenderPlayer( int controllerIndex )
{
	if( m_aliveState != ALIVE ){ return; }
	if( g_theCameraSystem->GetSplitScreenState() == VORONOI_SPLIT ) {
		RenderMarkers( controllerIndex );
	}
	__super::RenderActor();
	Vec2 forwardDirt = Vec2::ONE;
	forwardDirt.SetAngleDegrees( m_orientationDegrees );
	g_theRenderer->SetDiffuseTexture( nullptr );
	g_theRenderer->DrawLine( m_position, m_position + forwardDirt * 50.f, 0.1f, Rgba8::RED );
}

void Player::UpdateMarkers()
{
 	m_markerPos.clear();
	m_markerVertices.clear();
 	std::vector<CameraController*> controllers = g_theCameraSystem->GetCameraControllers();
	std::vector<Player*> players = m_map->GetPlayers();
	Vec2 cameraPos = controllers[m_index]->GetCameraPos();;
 	Polygon2 voronoiPoly = controllers[m_index]->GetVoronoiPolygon();
	std::vector<Vec2> worldPoints;
	voronoiPoly.GetAllVerticesInWorld( worldPoints );
 	for( int i =0; i < m_map->GetPlayerNum(); i++ ) {
 		if( i == m_index ){ continue; }
 		Vec2 opponentPos = m_map->GetPlayerPosWithIndex( i );
		voronoiPoly.SetCenter( cameraPos );
		if( voronoiPoly.IsPointInside( opponentPos ) ) {
			continue;
		}
		
 		Vec2 dirt = opponentPos - m_position;
		dirt.Normalize();
 		LineSegment2 line( m_position, opponentPos );
 		std::pair<Vec2, Vec2> intersectPoints = voronoiPoly.GetIntersectPointWithStraightLine( line );	
		Vec2 markerPos = Vec2::ZERO;
		if( IsPointForwardOfPoint2D( intersectPoints.first, m_position, dirt ) ) {
			markerPos = intersectPoints.first - dirt * 1.f;
		}
		else {
			markerPos = intersectPoints.second - dirt * 1.f;
		}
		Vec2 perpendicularDirt = Vec2( -dirt.y, dirt.x );
		m_markerVertices.push_back( Vertex_PCU( markerPos, players[i]->GetColor(), Vec2::ZERO ) );
		m_markerVertices.push_back( Vertex_PCU( markerPos - dirt + perpendicularDirt, players[i]->GetColor(), Vec2::ZERO ) );
		m_markerVertices.push_back( Vertex_PCU( markerPos - dirt - perpendicularDirt, players[i]->GetColor(), Vec2::ZERO ) );
 	}
}

void Player::RenderMarkers( int controllerIndex )
{
	if( controllerIndex == m_index ) {
		g_theRenderer->SetDiffuseTexture( nullptr );
		g_theRenderer->DrawVertexVector( m_markerVertices );
	}
}

void Player::TakeDamage( float damage )
{
	g_theCameraSystem->AddCameraShake( m_index, 0.5f );
	__super::TakeDamage( damage );
}

void Player::Die()
{
	SetAliveState( WAIT_FOR_DELETE );
	g_theCameraSystem->PrepareRemoveAndDestroyController( this );
}

int Player::GetPlayerIndex()
{
	const std::vector<Player*> players = m_map->GetPlayers();
	for( int i = 0; i < players.size(); i++ ) {
		if( players[i] == this ) {
			return i ;
		}
	}
	g_theConsole->DebugErrorf( "player not exist in get player index " );
	return -1;
}

void Player::SetMap( Map* map )
{
	m_map = map;
}

void Player::SetIndex( int index )
{
	m_index = index;
}

void Player::DisableInputForSeconds( float seconds )
{
	m_isAbleInput = false;
	g_theEventSystem->SetTimerByFunction( seconds, this, &Player::EnableInput );
	m_disableInputSeconds = seconds;
}

void Player::FakeDeadForSeconds( float seconds )
{
	m_aliveState = WAIT_FOR_REBORN;
	CameraController* controller = g_theCameraSystem->GetCameraControllerWithPlayer( this );
	controller->SetMultipleCameraStableFactorNotStableUntil( 2.f, 0.f );
	g_theCameraSystem->UpdateControllerMultipleFactor( 2.f );

	g_theEventSystem->SetTimerByFunction( seconds, this, &Player::Reborn );
	DisableInputForSeconds( 4.f );
}

bool Player::Reborn( EventArgs& args )
{
	UNUSED( args );
	Vec2 teleportPos = (Vec2)m_map->GetRandomInsideCameraNotSolidTileCoords( g_theCameraSystem->GetNoSplitCamera() );
	SetPosition( teleportPos );
	m_aliveState = ALIVE;
	g_theCameraSystem->UpdateControllerMultipleFactor( 2.f );
	return true;
}

bool Player::EnableInput( EventArgs& args )
{
	UNUSED(args);
	m_isAbleInput = true;
	return true;
}






