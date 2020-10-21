#include "CameraSystem.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern Game* g_theGame;

void CameraSystem::Startup()
{
	m_controllers.reserve( 4 );
	m_rng = new RandomNumberGenerator( 1 );
}

void CameraSystem::Shutdown()
{
	delete m_rng;
	m_rng = nullptr;
}

void CameraSystem::BeginFrame()
{

}

void CameraSystem::EndFrame()
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->m_player->GetAliveState() == READY_TO_DELETE_CONTROLLER ) {
			m_controllers[i]->m_player->SetAliveState( AliveState::READY_TO_DELETE_PLAYER );
			delete m_controllers[i];
			m_controllers.erase( m_controllers.begin() + i );
		}
	}
}

void CameraSystem::Update( float deltaSeconds )
{
	
	UpdateControllers( deltaSeconds );
	UpdateSplitScreenEffects( deltaSeconds );
	UpdateControllerCameras();
	UpdateDebugInfo();

}

void CameraSystem::UpdateControllers( float deltaSeconds )
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->Update( deltaSeconds );
	}
}

void CameraSystem::UpdateSplitScreenEffects( float deltaSeconds )
{
	switch( m_splitScreenState )
	{
		case NO_SPLIT_SCREEN: {
			UpdateNoSplitScreenEffect( deltaSeconds );
			break;
		}
		case NUM_OF_SPLIT_SCREEN_STATE:
			break;
	}
}

void CameraSystem::UpdateNoSplitScreenEffect( float deltaSeconds )
{
	if( m_controllers.size() == 0 ){ return; }
	std::vector<CameraController*> notInsideControllers;
	static int zoomRecoverWaitFrames = 0;
	switch( m_noSplitScreenStrat )
	{
		case NO_STRAT:
			break;
		case ZOOM_TO_FIT: {
			ZoomCameraToFitPlayers();
			break;
		}
		case KILL_AND_TELEPORT: {
			GetNotInsideControllers( notInsideControllers, -2.f );
			KillAndTeleportPlayers( notInsideControllers );
		}
	}
}

void CameraSystem::UpdateControllerCameras()
{
	if( m_controllers.size() == 0 ){ return; }

	if( m_splitScreenState == NO_SPLIT_SCREEN && m_controllers.size() > 1 ) {
		if( !m_noSplitCamera ) {
			m_noSplitCamera = g_theGame->m_gameCamera;
		}
		m_goalCameraPos = Vec2::ZERO;

		if( m_controllers.size() == 1 ) {
			m_goalCameraPos = m_controllers[0]->GetSmoothedGoalPos();
		}
		else {
			float totalFactor = 0.f;
			for( int i = 0; i < m_controllers.size(); i++ ) {
				totalFactor += m_controllers[i]->GetCurrentMultipleFactor();
			}
			for( int i = 0; i < m_controllers.size(); i++ ) {
				Vec2 smoothedPos = m_controllers[i]->GetSmoothedGoalPos();
				float multipleFactor = m_controllers[i]->GetCurrentMultipleFactor();
				m_goalCameraPos += ( m_controllers[i]->GetSmoothedGoalPos() * m_controllers[i]->GetCurrentMultipleFactor() );
			}
			m_goalCameraPos = m_goalCameraPos / totalFactor; 
		}
		m_noSplitCamera->SetPosition2D( m_goalCameraPos );
	}
	else {
		for( int i = 0; i < m_controllers.size(); i++ ) {
			m_controllers[i]->UpdateCameraPos();
		}
	}
}

void CameraSystem::DebugRender()
{
	DebugRenderControllers();
}

void CameraSystem::DebugRenderControllers()
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->DebugRender();
	}
}

std::string CameraSystem::GetCameraWindowStateText() const
{
	std::string result = "Camera window state :";
	switch( m_cameraWindowState )
	{
	case NO_CAMERA_WINDOW:
		return result + std::string( "No Camera Window" );
	case USE_CAMERA_WINDOW:
		return result + std::string( "Camera Window" );
	case NUM_OF_CAMERA_WINDOW_STATE:
		return result +std::string( "Num of Camera window" );
	}
	return "error camera window";
}

std::string CameraSystem::GetCameraSnappingStateText() const
{
	std::string result = "Camera snapping state :";
	
	switch( m_cameraSnappingState )
	{
	case NO_CAMERA_SNAPPING:
		return result + std::string( "No Camera Snapping " );
	case POSITION_SNAPPING:
		return result + std::string( "Position Snapping" );
	case POSITION_HORIZONTAL_LOCK:
		return result + std::string( "Horizontal lock" );
	case POSITION_VERTICAL_LOCK:
		return result + std::string( "Vertical lock" );
	case POSITION_LOCK:
		return result + std::string( "Position lock" );
	case NUM_OF_CAMERA_SNAPPING_STATE:
		return result +std::string( "Num of camera snapping" );
	}
	return " Error state";
}

std::string CameraSystem::GetCameraShakeStateText() const
{
	std::string result = "Camera shake state :";
	switch( m_cameraShakeState )
	{
	case POSITION_SHAKE:
		return result + std::string( "Position shake." );
	case ROTATION_SHAKE:
		return result + std::string( "Rotation shake." );
	case BLEND_SHAKE:
		return result + std::string( "Blend shake." );
	case NUM_OF_SHAKE_STATE:
		return result +std::string( "Num of camera Shake" );
	}
	return "Error state";
}

std::string CameraSystem::GetCameraFrameStateText() const
{
	std::string result = "Camera shake state : ";
	switch( m_cameraFrameState )
	{
	case NO_FRAMEING:
		return result + std::string( "No frame." );
	case FORWARD_FRAMING:
		return result + std::string( "Forward frame." );
	case PROJECTILE_FRAMING:
		return result + std::string( "Projectile frame." );
	case CUE_FRAMING:
		return result + std::string( "Cue frame." );
	case BLEND_FRAMING:
		return result + std::string( "Blend frame." );
	case NUM_OF_FRAME_STATE:
		return result + std::string( "Num of camera frame." );
	}
	return result + "Error state.";
}

std::string CameraSystem::GetSplitScreenStateText() const
{
	std::string result = "split screen state : ";
	switch( m_splitScreenState )
	{
	case NO_SPLIT_SCREEN:
		return result + std::string( "No split screen" );
	case NUM_OF_SPLIT_SCREEN_STATE:
		return result + std::string( "Num of split screen" );
	}
	return result + "Error state.";
}

std::string CameraSystem::GetNoSplitScreenStratText() const
{
	std::string result = "No split screen strat : ";
	switch( m_noSplitScreenStrat )
	{
	case NO_STRAT:
		return result + std::string( "No strat" );
	case ZOOM_TO_FIT:
		return result + std::string( "Zoom to fit" );
	case KILL_AND_TELEPORT:
		return result + std::string( "kill and teleport" );
	case NUM_OF_NO_SPLIT_SCREEN_STRAT:
		return result + std::string( "Number of no split screen strat" );
	}
	return result + "Error state.";
}

CameraController* CameraSystem::GetCameraControllerWithPlayer( Player* player )
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->m_player == player ){ return m_controllers[i]; }
	}
	return nullptr;
}

std::vector<CameraController*>& CameraSystem::GetCameraControllers()
{
	return m_controllers;
}

void CameraSystem::SetIsDebug( bool isDebug )
{
	m_isdebug = isDebug;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->SetIsDebug( isDebug );
	}
}

void CameraSystem::SetControllerSmooth( bool isSmooth )
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->SetIsSmooth( isSmooth );
	}
}

void CameraSystem::UpdateDebugInfo()
{
	std::string windowStateText		= GetCameraWindowStateText();
	std::string snappingStateText	= GetCameraSnappingStateText(); 
	std::string shakeStateText		= GetCameraShakeStateText();
	std::string frameStateText		= GetCameraFrameStateText();
	std::string splitStateText		= GetSplitScreenStateText();
	std::string splitStratText;
	switch( m_splitScreenState )
	{
	case NO_SPLIT_SCREEN:
		splitStratText = GetNoSplitScreenStratText();
		break;
	case NUM_OF_SPLIT_SCREEN_STATE:
		break;
	default:
		break;
	}


	Strings debugInfos;
	debugInfos.push_back( windowStateText );
	debugInfos.push_back( snappingStateText );
	debugInfos.push_back( shakeStateText );
	debugInfos.push_back( frameStateText );
	debugInfos.push_back( splitStateText );
	debugInfos.push_back( splitStratText );

	// coe debug
	if( m_noSplitCamera ) {
		float height = m_noSplitCamera->GetCameraHeight();
		std::string heightText = std::string( " Caemra height is " + std::to_string( height ));
		debugInfos.push_back( heightText );
	}

	DebugAddScreenLeftAlignStrings( 0.98f, 0.f, Rgba8( 255, 255, 255, 125), debugInfos );

	float posHeight = 0.4f;
	float posWidth = 0.5f;
	Vec4 pos[4] = { Vec4( 0.f, posHeight, 0.f, 0.f ),
					Vec4( posWidth, posHeight, 0.f, 0.f ),
					Vec4( 0.f, posHeight * 0.5f, 0.f, 0.f ),
					Vec4( posWidth, posHeight * 0.5f, 0.f, 0.f )
	} ;
 	for( int i = 0; i < m_controllers.size(); i++ ) {
 		m_controllers[i]->DebugCameraInfoAt( pos[i] );
 	}
}

void CameraSystem::CreateAndPushController( Player* player, Camera* camera )
{
	CameraController* tempCamController = new CameraController( this, player, camera );
	tempCamController->SetCameraWindowSize( Vec2( 12, 8 ) );
	if( m_controllers.size() == 0 ){
		camera->SetPosition( player->GetPosition() );
	}
	m_controllers.push_back( tempCamController );
	float smoothTime = 2.f;
	if( m_controllers.size() == 1 ){
		smoothTime = 0.1f;
	}
	UpdateControllerMultipleFactor( smoothTime );
}

void CameraSystem::PrepareRemoveAndDestroyController( Player const* player )
{
	float smoothTime = 2.f;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->m_player == player ) {
			m_controllers[i]->SetMultipleCameraStableFactorNotStableUntil( smoothTime, 0.f );
		}	
	}
	UpdateControllerMultipleFactor( smoothTime );
}

void CameraSystem::UpdateControllerMultipleFactor( float smoothTime )
{
	
	int num = GetValidPlayerNum();
	if( num == 0 ){ return; }
	float goalFactor = 1.f / (float)num ;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->m_player->GetAliveState() == ALIVE ) {
			m_controllers[i]->SetMultipleCameraStableFactorNotStableUntil( smoothTime, goalFactor );
		}
	}
}

int CameraSystem::GetValidPlayerNum()
{
	int result = 0;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->m_player->GetAliveState() == ALIVE ) {
			result++;
		}
	}
	return result;
}

bool CameraSystem::IsControllersInsideCamera()
{
	if( m_controllers.size() == 0 ){ return true; }
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();	
	AABB2 cameraBox{ cameraBottomLeft, cameraTopRight };
	for( int i = 0; i < m_controllers.size(); i++ ) {
	
	}
	return true;
}

void CameraSystem::GetNotInsideControllers( std::vector<CameraController*>& vec, float insidePaddingLength )
{
	vec.clear();
	if( m_noSplitCamera == nullptr ){ return; }
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();
	Vec2 padding			= Vec2( insidePaddingLength ); 
	AABB2 currentCameraWindow = AABB2( cameraBottomLeft + padding, cameraTopRight - padding );
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		Vec2 playerPos = tempController->m_playerPos;
		if( !IsPointInsideAABB2D( playerPos, currentCameraWindow ) ) {
			vec.push_back( tempController );
		}
	}
}

bool CameraSystem::CheckIfZoomStable( float stablePaddingLength )
{
	Vec2 stablePadding		= Vec2( stablePaddingLength );
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();
	AABB2 currentZoomStableWindow = AABB2( cameraBottomLeft + stablePadding, cameraTopRight - stablePadding );
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		Vec2 playerPos = tempController->m_playerPos;
		if( !IsPointInsideAABB2D( playerPos, currentZoomStableWindow ) ) {
			return true;
		}
	}
	return false;
}

void CameraSystem::KillAndTeleportPlayers( std::vector<CameraController*>& vec )
{
	if( vec.size() == 0 ){ return; }
	for( int i = 0; i < vec.size(); i++ ) {
		CameraController* tempController = vec[i];
		if( m_map == nullptr ) {
			m_map = g_theGame->GetCurrentMap();
		}
		if( tempController->m_player->GetAliveState() == ALIVE ) {
			tempController->m_player->FakeDeadForSeconds( 2.f );
		}
	}
}

void CameraSystem::ZoomCameraToFitPlayers()
{
	static int zoomRecoverWaitFrames = 0;
	std::vector<CameraController*> notInsideControllers;
	GetNotInsideControllers( notInsideControllers, m_notInsidePaddingLength );
	float goalZoomCameraHeight = 0.f;
	if( notInsideControllers.size() > 0 ) {
		goalZoomCameraHeight =	ComputeCameraHeight( notInsideControllers );
		float currentCameraHeight = m_noSplitCamera->GetCameraHeight();
		goalZoomCameraHeight = RangeMapFloat( 0.f, 1.f, currentCameraHeight, goalZoomCameraHeight, 0.1f );
		zoomRecoverWaitFrames = 0;
	}
	else if( m_noSplitCamera->GetCameraHeight() > m_idealCameraHeight && !CheckIfZoomStable( m_zoomStablePaddingLength ) ) {
		zoomRecoverWaitFrames++;
		float currentCameraHeight = m_noSplitCamera->GetCameraHeight();
		if( zoomRecoverWaitFrames > 10 ) {
			goalZoomCameraHeight = RangeMapFloat( 0.f, 1.f, currentCameraHeight, m_idealCameraHeight, 0.1f );
			goalZoomCameraHeight = ClampFloat( (currentCameraHeight - m_maxCameraDeltaHeightPerFrame), (currentCameraHeight + m_maxCameraDeltaHeightPerFrame), goalZoomCameraHeight );
		}
		else {
			goalZoomCameraHeight = currentCameraHeight;
		}
	}
	else {
		return;
	}
	Vec2 zoomCameraHalfDimension;
	float width = goalZoomCameraHeight * m_noSplitCamera->GetOutputAspectRatio();
	zoomCameraHalfDimension = Vec2( width / 2, goalZoomCameraHeight / 2 );
	m_noSplitCamera->SetOrthoView( -zoomCameraHalfDimension, zoomCameraHalfDimension, 1 );
}

float CameraSystem::ComputeCameraHeight( std::vector<CameraController*> const& vec )
{
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();
	float cameraHalfHeight = ( cameraTopRight.y - cameraBottomLeft.y ) / 2;
	float newHalfHeight = cameraHalfHeight;
	
	for( int i = 0; i < vec.size(); i++ ) {
		CameraController* tempController = vec[i];
		Vec2 playerPos = tempController->m_playerPos;
		Vec2 disp = m_noSplitCamera->GetPosition2D() - playerPos;
		float aspectRatio = m_noSplitCamera->GetOutputAspectRatio();
		float tempNewHalfHeight = (abs(disp.y) > (abs(disp.x) / aspectRatio)) ? abs(disp.y) : (abs(disp.x) / aspectRatio);
		tempNewHalfHeight += 2.f; // add for player texture
		if( newHalfHeight < tempNewHalfHeight ) {
			newHalfHeight = tempNewHalfHeight;
		}
	}
	return ( newHalfHeight * 2 );
}

void CameraSystem::SetCameraWindowState( CameraWindowState newState )
{
	m_cameraWindowState = newState;
}

void CameraSystem::SetCameraSnappingState( CameraSnappingState newState )
{
	m_cameraSnappingState = newState;
}

void CameraSystem::SetCameraShakeState( CameraShakeState newState )
{
	m_cameraShakeState = newState;
}

void CameraSystem::SetCameraFrameState( CameraFrameState newState )
{
	m_cameraFrameState = newState;
}

void CameraSystem::SetSplitScreenState( SplitScreenState newState )
{
	m_splitScreenState = newState;
}

void CameraSystem::SetNoSplitScreenStrat( NoSplitScreenStrat newStrat )
{
	m_noSplitScreenStrat = newStrat;
}

void CameraSystem::SetMap( Map* map )
{
	m_map = map;
}

void CameraSystem::AddCameraShake( int index, float shakeTrauma )
{
	if( index < m_controllers.size() ) {
		m_controllers[index]->AddTrauma( shakeTrauma );
	}
}

