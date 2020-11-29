#include "CameraSystem.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

extern Game*			g_theGame;
extern Camera*			g_gameCamera;
extern RenderContext*	g_theRenderer;

void CameraSystem::Startup()
{
	m_controllers.reserve( 4 );
	m_rng = new RandomNumberGenerator( 1 );
	m_noSplitCamera = g_gameCamera;
	m_noSplitCamera->EnableClearColor( Rgba8::DARK_GRAY );
	m_multipleCameraShader = g_theRenderer->GetOrCreateShader( "data/Shader/multipleCamera.hlsl" );
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
		m_controllers[i]->EndFrame();
		if( m_controllers[i]->m_player->GetAliveState() == READY_TO_DELETE_CONTROLLER ) {
			m_controllers[i]->m_player->SetAliveState( AliveState::READY_TO_DELETE_PLAYER );
			delete m_controllers[i];
			m_controllers.erase( m_controllers.begin() + i );
			for( int j = i; j < m_controllers.size(); j++ ) {
				m_controllers[j]->SetIndex( j );
			}
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

void CameraSystem::Render()
{
	if( m_splitScreenState == NO_SPLIT_SCREEN ) {
		if( m_noSplitCamera == nullptr ) {
			m_noSplitCamera = g_theGame->m_gameCamera;
		}
		g_theRenderer->BeginCamera( m_noSplitCamera );
		g_theGame->RenderGame();
		g_theRenderer->EndCamera();
	}
	else if( m_splitScreenState == VORONOI_SPLIT ) {
		
		AABB2 splitCheckBox	= GetSplitCheckBox();
		if( !DoesNeedSplitScreen( splitCheckBox ) ) {
			g_theRenderer->BeginCamera( m_noSplitCamera );
			g_theGame->RenderGame();
			g_theRenderer->EndCamera();
		}
		else {
// 			g_theRenderer->BeginCamera( m_noSplitCamera );
// 			g_theGame->RenderGame();
// 			if( m_controllers.size() == 2 ){
// 				g_theRenderer->DrawPolygon2D( m_controllers[0]->GetVoronoiPoly(), Rgba8::RED );
// 				g_theRenderer->DrawPolygon2D( m_controllers[1]->GetVoronoiPoly(), Rgba8::RED );
// 			}
// 			g_theRenderer->EndCamera();

			
			for( int i = 0; i < m_controllers.size(); i++ ) { // TODO test for -1
				m_controllers[i]->Render();
			}

			g_theRenderer->BeginCamera( m_noSplitCamera );
			g_theRenderer->BindShader( m_multipleCameraShader );

			for( int i = 0; i < m_controllers.size(); i++ ) {
				g_theRenderer->SetDiffuseTexture( m_controllers[i]->GetColorTarget() );
				g_theRenderer->SetMaterialTexture( m_controllers[i]->GetStencilTexture() );
				g_theRenderer->SetOffsetBuffer( m_controllers[i]->GetOffsetBuffer(), 0 );
				g_theRenderer->m_context->Draw( 3, 0 );
			}
			g_theRenderer->EndCamera();

			for( int i = 0; i < m_controllers.size(); i++ ) {
				m_controllers[i]->ReleaseRenderTarget();
			}
		}
	}
	else {
		// split screen and multiple camera
		for( int i = 0; i < m_controllers.size(); i++ ) {
			m_controllers[i]->Render();
		}

		g_theRenderer->BeginCamera( m_noSplitCamera ); 
		g_theRenderer->BindShader( m_multipleCameraShader );
		
		for( int i = 0; i < m_controllers.size(); i++ ) {
			g_theRenderer->SetDiffuseTexture( m_controllers[i]->GetColorTarget() );
			g_theRenderer->SetMaterialTexture( m_controllers[i]->GetStencilTexture() );
			g_theRenderer->SetOffsetBuffer( m_controllers[i]->GetOffsetBuffer(), 0 );
			g_theRenderer->m_context->Draw( 3, 0 );
		}
		g_theRenderer->EndCamera();					   

		for( int i = 0; i < m_controllers.size(); i++ ) {
			m_controllers[i]->ReleaseRenderTarget();
		}

	}
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
		}
		break;
		case VORONOI_SPLIT: {
			UpdateVoronoiSplitScreenEffect( deltaSeconds );
		}
		break;
		case NUM_OF_SPLIT_SCREEN_STATE:
			break;
	}
}

void CameraSystem::UpdateNoSplitScreenEffect( float deltaSeconds )
{
	UNUSED (deltaSeconds);
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

void CameraSystem::UpdateVoronoiSplitScreenEffect( float delteSeconds )
{
	AABB2 worldCameraBox = GetWorldCameraBox();
	AABB2 splitCheckBox	= GetSplitCheckBox();
	if( DoesNeedSplitScreen( splitCheckBox ) ) {
		ConstructVoronoiDiagramForControllers( worldCameraBox, splitCheckBox );
	}
	else {
		Vec2 pos = GetAverageCameraPosition();
		 m_noSplitCamera->SetPosition2D( pos );
	}

}

void CameraSystem::UpdateControllerCameras()
{
	if( m_controllers.size() == 0 ){ return; }

	if( m_splitScreenState == NO_SPLIT_SCREEN ) {
		if( !m_noSplitCamera ) {
			m_noSplitCamera = g_theGame->m_gameCamera;
		}

		m_goalCameraPos = Vec2::ZERO;
		float totalFactor = GetTotalFactor();

		for( int i = 0; i < m_controllers.size(); i++ ) {
			Vec2 smoothedPos = m_controllers[i]->GetSmoothedGoalPos();
			m_goalCameraPos += ( m_controllers[i]->GetSmoothedGoalPos() * m_controllers[i]->GetCurrentMultipleFactor() );
		}
		
		m_goalCameraPos = m_goalCameraPos / totalFactor; 
		m_noSplitCamera->SetPosition2D( m_goalCameraPos );
	}
	
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->UpdateCameraPos();
	}
	
}

void CameraSystem::DebugRender()
{
	// debug draw intersect points
	for( int i = 0; i < m_testIntersectPoints.size(); i++ ) {
		g_theRenderer->SetMaterialTexture( nullptr );
		g_theRenderer->DrawCircle( m_testIntersectPoints[i], 0.5f, 1.f, Rgba8::RED );
	}
	DebugRenderControllers();
}

void CameraSystem::DebugRenderControllers()
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->DebugRender();
	}
}

bool CameraSystem::DoesNeedSplitScreen( AABB2 cameraBox ) const
{
	for( auto iter = m_controllers.begin(); iter != m_controllers.end(); ++iter ) {
		Vec2 playerPos = (*iter)->m_player->GetPosition();
		if( !IsPointInsideAABB2D( playerPos, cameraBox ) ) {
			return true;
		}
	}
	return false;
}

float CameraSystem::GetTotalFactor() const
{
	float totalFactor = 0.f;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		totalFactor += m_controllers[i]->GetCurrentMultipleFactor();
	}
	return totalFactor;
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
	case AXIS_ALIGNED_SPLIT:
		return result + std::string( "axis aligned split screen" );
	case VORONOI_SPLIT:
		return result + std::string( "voronoi split screen" );
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

Vec2 CameraSystem::GetAverageCameraPosition()
{
	Vec2 totalPos = Vec2::ZERO;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		totalPos += tempController->GetCameraPos();
	}
	totalPos /= (int)m_controllers.size();
	return totalPos;
}

AABB2 CameraSystem::GetWorldCameraBox()
{
	Vec2 boxCenter = GetAverageCameraPosition();
	Vec2 boxDimension = g_gameCamera->Get2DDimension();
	AABB2 worldCameraBox = AABB2( boxCenter, boxDimension.x, boxDimension.y );
	return worldCameraBox;
}

AABB2 CameraSystem::GetSplitCheckBox()
{
	Vec2 boxCenter = GetAverageCameraPosition();
	AABB2 worldCameraBox = AABB2( boxCenter, GAME_CHECK_WIDTH, GAME_CHECK_HEIGHT );
	return worldCameraBox;
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
	};
	if( m_isdebug ) {
 		for( int i = 0; i < m_controllers.size(); i++ ) {
 			m_controllers[i]->DebugCameraInfoAt( pos[i] );
 		}
	}
}

void CameraSystem::CreateAndPushController( Player* player )
{
	Camera* tempCamera = Camera::CreateOrthographicCamera( g_theRenderer, Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	CameraController* tempCamController = new CameraController( this, player, tempCamera );
	tempCamController->SetCameraWindowSize( Vec2( 12, 8 ) );
	tempCamera->SetPosition( player->GetPosition() );
	m_controllers.push_back( tempCamController );
	tempCamController->SetIndex( m_controllers.size() - 1 );
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

void CameraSystem::ConstructVoronoiDiagramForControllers( AABB2 worldCameraBox, AABB2 splitCheckBox )
{
	if( m_controllers.size() == 2 ) {
		ConstructVoronoiDiagramForTwoControllers( worldCameraBox, splitCheckBox );
	}
	else if( m_controllers.size() == 3 ) {
		ConstructVoronoiDiagramForThreeControllers( worldCameraBox, splitCheckBox );
	}
	else if( m_controllers.size() == 4 ) {
		ConstructVoronoiDiagramForFourControllers( worldCameraBox, splitCheckBox );
	}
}

void CameraSystem::ConstructVoronoiDiagramForTwoControllers( AABB2 worldCameraBox, AABB2 splitCheckBox )
{
	Vec2 player1CameraPos = m_controllers[0]->m_cameraPos;
	Vec2 player2CameraPos = m_controllers[1]->m_cameraPos;
	Vec2 dispAB		= player2CameraPos - player1CameraPos;
	Vec2 centerPos	= (player1CameraPos + player2CameraPos) / 2;
	Vec2 dirtAB		= dispAB.GetNormalized();
	Vec2 dirtPerpendicularAB = Vec2( -dirtAB.y, dirtAB.x );
	LineSegment2 perpendicularLine = LineSegment2( centerPos, centerPos + dirtPerpendicularAB * 5.f );
	//m_testIntersectPoints = GetIntersectionPointOfLineAndAABB2( perpendicularLine, worldCameraBox );
	std::vector<Vec2> pointsA = GetIntersectionPointOfLineAndAABB2( perpendicularLine, worldCameraBox );
	std::vector<Vec2> pointsB = GetIntersectionPointOfLineAndAABB2( perpendicularLine, worldCameraBox );
	Vec2 worldCameraBoxCorners[4];
	worldCameraBox.GetCornerPositions( worldCameraBoxCorners );
	for( int i = 0; i < 4; i++ ) {
		float distToPointA = GetDistance2D( player1CameraPos, worldCameraBoxCorners[i] );
		float distToPointB = GetDistance2D( player2CameraPos, worldCameraBoxCorners[i] );
		if( distToPointA < distToPointB ) {
			pointsA.push_back( worldCameraBoxCorners[i] );
		}
		else {
			pointsB.push_back( worldCameraBoxCorners[i] );
		}
	}
	Polygon2 polygonA = Polygon2::MakeConvexFromPointCloud( pointsA );
	Polygon2 polygonB = Polygon2::MakeConvexFromPointCloud( pointsB );
	Vec2 player1InCheckBox = splitCheckBox.GetNearestPoint( player1CameraPos );
	Vec2 player2InCheckBox = splitCheckBox.GetNearestPoint( player2CameraPos );
	Vec2 dispCheckBoxToPlayer1 = player1CameraPos - player1InCheckBox;
	Vec2 dispCheckBoxToPlayer2 = player2CameraPos - player2InCheckBox;
	polygonA.m_center += dispCheckBoxToPlayer1;
	polygonB.m_center += dispCheckBoxToPlayer2;

	m_controllers[0]->SetVoronoiOffset( dispCheckBoxToPlayer1 );
	m_controllers[1]->SetVoronoiOffset( dispCheckBoxToPlayer2 );
	m_controllers[0]->SetVoronoiPolygon( polygonA );
	m_controllers[1]->SetVoronoiPolygon( polygonB );
}

void CameraSystem::ConstructVoronoiDiagramForThreeControllers( AABB2 worldCameraBox, AABB2 splitCheckBox )
{
	Vec2 playerCameraPosA = m_controllers[0]->GetCameraPos();
	Vec2 playerCameraPosB = m_controllers[1]->GetCameraPos();
	Vec2 playerCameraPosC = m_controllers[2]->GetCameraPos();
	
	Vec2 pointA	= splitCheckBox.GetNearestPoint( playerCameraPosA );
	Vec2 pointB = splitCheckBox.GetNearestPoint( playerCameraPosB );
	Vec2 pointC = splitCheckBox.GetNearestPoint( playerCameraPosC );

	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( pointA, pointB );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( pointB, pointC );
	LineSegment2 PB_CA = GetPerpendicularBisectorOfTwoPoints( pointC, pointA );
	// Check if abc colinear
	LineSegment2 AB = LineSegment2( pointA, pointB );
	std::vector<Vec2> intersectPointsA;
	std::vector<Vec2> intersectPointsB;
	std::vector<Vec2> intersectPointsC;
		
	if( AB.IsPointMostlyInStraightLine( pointC ) ) {
		// abc colinear
		Vec2 disp_AB = pointB - pointA;
		Vec2 disp_AC = pointC - pointA;
		float CP_AB_AC = CrossProduct2D( disp_AB, disp_AC );
		float CP_AB_AB = CrossProduct2D( disp_AB, disp_AB );
		if( CP_AB_AC < 0 ) {
			// order is CAB
			GetVoronoiPointsWithThreePointsInCollinearOrder( pointC, pointA, pointB, worldCameraBox, intersectPointsC, intersectPointsA, intersectPointsB );
		}
		else if( CP_AB_AC < CP_AB_AB ){
			// order is acb
			GetVoronoiPointsWithThreePointsInCollinearOrder( pointA, pointC, pointB, worldCameraBox, intersectPointsA, intersectPointsC, intersectPointsB );
		}
		else{
			// order is abc
			GetVoronoiPointsWithThreePointsInCollinearOrder( pointA, pointB, pointC, worldCameraBox, intersectPointsA, intersectPointsB, intersectPointsC );
		}
	}
	else {
		GetVoronoiPointsWithThreePointsNotCollinear( pointA, pointB, pointC, worldCameraBox, intersectPointsA, intersectPointsB, intersectPointsC );
	}
	Polygon2 polygonA = Polygon2::MakeConvexFromPointCloud( intersectPointsA );
	Polygon2 polygonB = Polygon2::MakeConvexFromPointCloud( intersectPointsB );
	Polygon2 polygonC = Polygon2::MakeConvexFromPointCloud( intersectPointsC );
	m_controllers[0]->SetVoronoiPolygon( polygonA );
	m_controllers[1]->SetVoronoiPolygon( polygonB );
	m_controllers[2]->SetVoronoiPolygon( polygonC );
	m_controllers[0]->SetVoronoiOffset( Vec2::ZERO );
	m_controllers[0]->SetVoronoiOffset( Vec2::ZERO );
	m_controllers[0]->SetVoronoiOffset( Vec2::ZERO );
}

void CameraSystem::ConstructVoronoiDiagramForFourControllers( AABB2 worldCameraBox, AABB2 splitCheckBox )
{

}

void CameraSystem::GetVoronoiPointsWithThreePointsInCollinearOrder( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, std::vector<Vec2>& pointsA, std::vector<Vec2>& pointsB, std::vector<Vec2>& pointsC )
{
	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( a, b );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( b, c );
	// abc same line in order a b c 
		// point order c a b 
	pointsA = GetIntersectionPointOfLineAndAABB2( PB_AB, worldCameraBox );
	pointsC = GetIntersectionPointOfLineAndAABB2( PB_BC, worldCameraBox );
	for( int i = 0; i < pointsA.size(); i++ ) {
		pointsB.push_back( pointsA[i] );
		pointsB.push_back( pointsC[i] );
	}

	Vec2 worldCameraBoxCorners[4];
	worldCameraBox.GetCornerPositions( worldCameraBoxCorners );
	for( int i = 0; i < 4; i++ ) {
		float distToPointA = GetDistance2D( a, worldCameraBoxCorners[i] );
		float distToPointC = GetDistance2D( c, worldCameraBoxCorners[i] );
		if( distToPointA < distToPointC ) {
			pointsA.push_back( worldCameraBoxCorners[i] );
		}
		else {
			pointsC.push_back( worldCameraBoxCorners[i] );
		}
	}
}

void CameraSystem::GetVoronoiPointsWithThreePointsNotCollinear( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, std::vector<Vec2>& pointsA, std::vector<Vec2>& pointsB, std::vector<Vec2>& pointsC )
{
	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( a, b );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( b, c );
	LineSegment2 PB_AC = GetPerpendicularBisectorOfTwoPoints( a, c );
	Vec2 centerABC = GetIntersectionPointOfTwoLines( PB_AB, PB_AC );
	std::vector<Vec2> PBIntersectWithBoxPoints_AB = GetIntersectionPointOfLineAndAABB2( PB_AB, worldCameraBox );
	std::vector<Vec2> PBIntersectWithBoxPoints_AC = GetIntersectionPointOfLineAndAABB2( PB_AC, worldCameraBox );
	std::vector<Vec2> PBIntersectWithBoxPoints_BC = GetIntersectionPointOfLineAndAABB2( PB_BC, worldCameraBox );

	pointsA = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( a, b, c, worldCameraBox, PBIntersectWithBoxPoints_AB, PBIntersectWithBoxPoints_AC );
	pointsB = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( b, a, c, worldCameraBox, PBIntersectWithBoxPoints_AB, PBIntersectWithBoxPoints_BC );
	pointsC = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( c, a, b, worldCameraBox, PBIntersectWithBoxPoints_AC, PBIntersectWithBoxPoints_BC );

	if( worldCameraBox.IsPointInside( centerABC ) ) {
		pointsA.push_back( centerABC );
		pointsB.push_back( centerABC );
		pointsC.push_back( centerABC );
	}
}

std::vector<Vec2> CameraSystem::GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( Vec2 point, Vec2 helpPointA, Vec2 helpPointB, AABB2 worldCameraBox, std::vector<Vec2> PBAPoints, std::vector<Vec2> PBBPoints )
{
	std::vector<Vec2> result;
	// get rid of far PB intersect points with box
	for( int i = 0; i < PBAPoints.size(); i++ ) {
		Vec2 tempPBPoint = PBAPoints[i];
		float distToPoint = GetDistance2D( point, tempPBPoint );
		float distToHelpPointA = GetDistance2D( point, helpPointA );
		float distToHelpPointB = GetDistance2D( point, helpPointB );
		if( distToPoint < distToHelpPointA && distToPoint <= distToHelpPointB ) {
			result.push_back( tempPBPoint );
		}
	}

	for( int i = 0; i < PBBPoints.size(); i++ ) {
		Vec2 tempPBPoint = PBBPoints[i];
		float distToPoint = GetDistance2D( point, tempPBPoint );
		float distToHelpPointA = GetDistance2D( point, helpPointA );
		float distToHelpPointB = GetDistance2D( point, helpPointB );
		if( distToPoint < distToHelpPointA && distToPoint <= distToHelpPointB ) {
			result.push_back( tempPBPoint );
		}
	}
	// Get nearest AABB point
	Vec2 worldCameraBoxCorners[4];
	worldCameraBox.GetCornerPositions( worldCameraBoxCorners );
	for( int i = 0; i < 4; i++ ) {
		float distToPoint	= GetDistance2D( point, worldCameraBoxCorners[i] );
		float distToPointA	= GetDistance2D( helpPointA, worldCameraBoxCorners[i] );
		float distToPointB	= GetDistance2D( helpPointB, worldCameraBoxCorners[i] );
		if( distToPoint < distToPointA && distToPointA < distToPointB ) {
			result.push_back( worldCameraBoxCorners[i] );
		}
	}
	return result;
}

