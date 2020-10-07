#include "CameraSystem.hpp"
#include "Game/Player.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

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
	std::vector<CameraController*> notInsideControllers;
	switch( m_splitScreenState )
	{
		case NO_SPLIT_SCREEN: {
			GetNotInsideControllers( notInsideControllers );
			switch( m_noSplitScreenStrat )
			{
				case NO_STRAT:
					break;
				case ZOOM_TO_FIT: {
					float zoomCameraHeight = 0.f;
					if( notInsideControllers.size() > 0 ) {
						zoomCameraHeight =	ComputeCameraHeight( notInsideControllers );
						float currentCameraHeight = m_noSplitCamera->GetCameraHeight();
						//zoomCameraHeight = RangeMapFloat( 0.f, 1.f, currentCameraHeight, zoomCameraHeight, 0.1f );
					}
					else if( m_noSplitCamera->GetCameraHeight() > m_idealCameraHeight ){
						zoomCameraHeight = m_noSplitCamera->GetCameraHeight();
						zoomCameraHeight = RangeMapFloat( 0.f, 1.f, zoomCameraHeight, m_idealCameraHeight, 0.1f );
					}
					else {
						break;
					}
					Vec2 zoomCameraHalfDimension;
					float width = zoomCameraHeight * m_noSplitCamera->GetOutputAspectRatio();
					zoomCameraHalfDimension = Vec2( width / 2, zoomCameraHeight / 2 );
					m_noSplitCamera->SetOrthoView( -zoomCameraHalfDimension, zoomCameraHalfDimension, 1 );
					break;
				}
			}
			break;
		}
		case NUM_OF_SPLIT_SCREEN_STATE:
			break;
	}
}

void CameraSystem::UpdateControllerCameras()
{
	if( m_controllers.size() == 0 ){ return; }

	if( m_splitScreenState == NO_SPLIT_SCREEN ) {
		if( !m_noSplitCamera ) {
			m_noSplitCamera = m_controllers[0]->m_camera;
		}
		m_goalCameraPos = Vec2::ZERO;
		for( int i = 0; i < m_controllers.size(); i++ ) {
			m_goalCameraPos += ( m_controllers[i]->GetSmoothedGoalPos() / ( (int)m_controllers.size() ) );
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
	case NUM_OF_NO_SPLIT_SCREEN_STRAT:
		return result + std::string( "Number of no split screen strat" );
	}
	return result + "Error state.";
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

	DebugAddScreenLeftAlignStrings( 0.98f, 0.f, Rgba8::RED, debugInfos );
}

void CameraSystem::CreateAndPushController( Player* player, Camera* camera )
{
	CameraController* tempCamController = new CameraController( this, player, camera );
	tempCamController->SetCameraWindowSize( Vec2( 12, 8 ) );
	camera->SetPosition( player->GetPosition() );
	m_controllers.push_back( tempCamController );
}

void CameraSystem::CheckIfZoom()
{
	if( !IsControllersInsideCamera() ) {

	}
}

bool CameraSystem::IsControllersInsideCamera()
{
	if( m_controllers.size() == 0 ){ return true; }
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();	
	AABB2 cameraBox{ cameraBottomLeft, cameraTopRight };
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		
	}
}

void CameraSystem::GetNotInsideControllers( std::vector<CameraController*>& vec )
{
	vec.clear();
	if( m_noSplitCamera == nullptr ){ return; }
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();
	Vec2 padding			= Vec2( 1.f ); 
	AABB2 currentCameraWindow = AABB2( cameraBottomLeft + padding, cameraTopRight - padding );
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		Vec2 playerPos = tempController->m_playerPos;
		if( !IsPointInsideAABB2D( playerPos, currentCameraWindow ) ) {
			vec.push_back( tempController );
		}
	}
}

float CameraSystem::ComputeCameraHeight( std::vector<CameraController*> const& vec )
{
	Vec2 cameraBottomLeft	= m_noSplitCamera->GetBottomLeftWorldPos2D();
	Vec2 cameraTopRight		= m_noSplitCamera->GetTopRightWorldPos2D();
	float cameraHalfHeight = ( cameraTopRight.y - cameraBottomLeft.y ) / 2;
	float newHalfHeight = cameraHalfHeight;
	
	for( int i = 0; i < vec.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		Vec2 playerPos = tempController->m_playerPos;
		Vec2 disp = m_noSplitCamera->GetPosition2D() - playerPos;
		float aspectRatio = m_noSplitCamera->GetOutputAspectRatio();
		float tempNewHalfHeight = (abs(disp.y) > (abs(disp.x) / aspectRatio)) ? abs(disp.y) : (abs(disp.x) / aspectRatio);
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

void CameraSystem::AddCameraShake( int index, float shakeTrauma )
{
	if( index < m_controllers.size() ) {
		m_controllers[index]->AddTrauma( shakeTrauma );
	}
}

