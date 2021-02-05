#include <set>
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
	if( m_isdebug ) {
		g_theRenderer->BeginCamera( m_noSplitCamera );
		g_theGame->RenderGame();
		DebugRender();
// 		if(m_splitScreenState == VORONOI_SPLIT ) {
// 			//g_theRenderer->BeginCamera( m_noSplitCamera );
// 			//g_theGame->RenderGame();
// 			//for( int i = 0; i < m_controllers.size(); i++ ) {
// 			//	// Draw points
// 			//	// draw lines
// 			//	//Vec2 position = m_controllers[i]->m_player->GetPosition();
// 			//	//g_theRenderer->DrawCircle( Vec3( position ), 0.5f, 0.5f, Rgba8::BLACK );
// 			//	//g_theRenderer->DrawPolygon2D( m_controllers[i]->GetVoronoiPoly(), Rgba8( 255, 0, 0, 50 ) );
// 			//	//g_theRenderer->DrawPolygon2D( m_controllers[1]->GetVoronoiPoly(), Rgba8::RED );
// 			//}
// 		}
		g_theRenderer->EndCamera();
	}
	else {
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

void CameraSystem::UpdateVoronoiSplitScreenEffect( float deltaSeconds )
{
	UNUSED( deltaSeconds ); 
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
	g_theRenderer->SetMaterialTexture( nullptr );
	float thick = 0.1f;
	float radius = 0.5f;
	float intersectPointThick = 0.1f;
	float intersectPointRadius = 0.1f;

	char alpha = 50;
	g_theRenderer->DrawCircle( m_playerDebugPosA, radius, thick, Rgba8( 255, 0, 0, alpha ) );
	g_theRenderer->DrawCircle( m_playerDebugPosB, radius, thick, Rgba8( 0, 255, 0, alpha ) );
	g_theRenderer->DrawCircle( m_playerDebugPosC, radius, thick, Rgba8( 0, 0, 255, alpha ) );

	g_theRenderer->DrawPolygon2D( m_DebugPolyA, Rgba8( 255, 0, 0, alpha ) );
	g_theRenderer->DrawPolygon2D( m_DebugPolyB, Rgba8( 0, 255, 0, alpha ) );
	g_theRenderer->DrawPolygon2D( m_DebugPolyC, Rgba8( 0, 0, 255, alpha ) );
	g_theRenderer->DrawPolygon2D( m_DebugPolyD, Rgba8( 1, 1, 1, 255 ) );

// 	for( int i = 0; i < m_debugLineIntersectWithBoxPointsAB.size(); i++ ) {
// 		g_theRenderer->DrawCircle( m_debugLineIntersectWithBoxPointsAB[i],intersectPointRadius, thick, Rgba8::YELLOW );
// 	}
// 	for( int i = 0; i < m_debugLineIntersectWithBoxPointsBC.size(); i++ ) {
// 		g_theRenderer->DrawCircle( m_debugLineIntersectWithBoxPointsBC[i], intersectPointRadius, thick, Rgba8::CYAN );
// 	}
// 	for( int i = 0; i < m_debugLineIntersectWithBoxPointsAB.size(); i++ ) {
// 		g_theRenderer->DrawCircle( m_debugLineIntersectWithBoxPointsAC[i], intersectPointRadius, thick, Rgba8::MAGENTA );
// 	}

// 	if( m_debugAfterDeletePoints.size() > 0 ) {
// 		for( int j = 0; j < m_debugAfterDeletePoints[0].size(); j++ ) {
// 			g_theRenderer->DrawCircle( m_debugAfterDeletePoints[0][j], 0.3f, 0.1f, Rgba8::RED );
// 		}
// 	}
// 	if( m_debugAfterDeletePoints.size() > 1 ) {
// 		for( int j = 0; j < m_debugAfterDeletePoints[1].size(); j++ ) {
// 			g_theRenderer->DrawCircle( m_debugAfterDeletePoints[1][j], 0.3f, 0.1f, Rgba8::GREEN );
// 		}
// 	}
// 	if( m_debugAfterDeletePoints.size() > 2 ) {
// 		for( int j = 0; j < m_debugAfterDeletePoints[2].size(); j++ ) {
// 			g_theRenderer->DrawCircle( m_debugAfterDeletePoints[2][j], 0.3f, 0.1f, Rgba8::BLUE );
// 		}
// 	}
	

// 	for( int i = 0; i < m_debugPBLines.size(); i++ ) {
// 		if( i < 3 ) {
// 			g_theRenderer->DrawLine( m_debugPBLines[i], thick, Rgba8::WHITE );
// 		}
// 		else {
// 			g_theRenderer->DrawLine( m_debugPBLines[i], thick, Rgba8::BLACK );
// 		}
// 	}
	for( int i = 0; i < m_debugIntersectPointsA.size(); i++ ) {
		g_theRenderer->DrawCircle( m_debugIntersectPointsA[i], intersectPointRadius, thick, Rgba8::RED );
	}
	for( int i = 0; i < m_debugIntersectPointsB.size(); i++ ) {
		g_theRenderer->DrawCircle( m_debugIntersectPointsB[i], intersectPointRadius + 0.3f, thick, Rgba8::GREEN );
	}
	for( int i = 0; i < m_debugIntersectPointsC.size(); i++ ) {
		g_theRenderer->DrawCircle( m_debugIntersectPointsC[i], intersectPointRadius + 0.5f, thick, Rgba8::BLUE );
	}
	for( int i = 0; i < m_debugIntersectPointsX.size(); i++ ) {
		g_theRenderer->DrawCircle( m_debugIntersectPointsX[i], intersectPointRadius + 0.7f, thick, Rgba8::BLACK );
	}

	AABB2 splitCheckBox = GetSplitCheckBox();
	AABB2 worldCameraBox = GetWorldCameraBox();
	g_theRenderer->DrawAABB2D( splitCheckBox, Rgba8( 255, 0, 0, 50 ) );
	g_theRenderer->DrawAABB2D( worldCameraBox, Rgba8( 0, 0, 255, 50 ) );

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

std::string CameraSystem::GetDebugModeText() const {
	std::string result = "No split screen strat : ";
	switch( m_debugMode )
	{
	case CONTROLLER_INFO:
		return result + std::string( "Controller Info" );
	case HELP_MODE:
		return result + std::string( "Help Mode" );
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
	totalPos /= (float)m_controllers.size();
	return totalPos;
}

AABB2 CameraSystem::GetWorldCameraBox()
{
	Vec2 boxCenter = GetAverageCameraPosition();
	Vec2 boxDimension = g_gameCamera->Get2DDimension();
	int testOffset = 5.f;
	AABB2 worldCameraBox = AABB2( boxCenter, boxDimension.x - testOffset, boxDimension.y - testOffset ); //TODO: clear test offset
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

void CameraSystem::SetDebugMode( DebugMode newMode )
{
	m_debugMode = newMode;
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
	std::string debugText			= "Press F1 to enable debug mode.";
	std::string windowStateText		= "Press F2 to change: " + GetCameraWindowStateText();
	std::string snappingStateText	= "Press F3 to change: " + GetCameraSnappingStateText(); 
	std::string shakeStateText		= "Press F4 to change: " + GetCameraShakeStateText();
	std::string frameStateText		= "Press F5 to change: " + GetCameraFrameStateText();
	std::string splitStateText		= "Press F6 to change: " + GetSplitScreenStateText();
	std::string splitStratText		= "Press F7 to change: ";
	std::string debugModeText		= "Press F9 to change: " + GetDebugModeText();

	std::string tutorialText = "Press p to create player. Press e to create enemy.";
	switch( m_splitScreenState )
	{
	case NO_SPLIT_SCREEN:
		splitStratText += GetNoSplitScreenStratText();
		break;
	case NUM_OF_SPLIT_SCREEN_STATE:
		break;
	default:
		splitStratText += "Split screen";
		break;
	}


	Strings debugInfos;
	debugInfos.push_back( debugText );
	debugInfos.push_back( windowStateText );
	debugInfos.push_back( snappingStateText );
	debugInfos.push_back( shakeStateText );
	debugInfos.push_back( frameStateText );
	debugInfos.push_back( splitStateText );
	debugInfos.push_back(debugModeText); 
	debugInfos.push_back( splitStratText );

	// coe debug
	if( m_noSplitCamera ) {
		float height = m_noSplitCamera->GetCameraHeight();
		std::string heightText = std::string( " Caemra height is " + std::to_string( height ));
		debugInfos.push_back( heightText );
	}

	DebugAddScreenLeftAlignStrings( 0.98f, 0.f, Rgba8( 255, 255, 255, 125), debugInfos );

	if( m_isdebug ) {
		if( m_debugMode == CONTROLLER_INFO ) {
			float posHeight = 0.4f;
			float posWidth = 0.5f;
			Vec4 pos[4] ={ Vec4( 0.f, posHeight, 0.f, 0.f ),
				Vec4( posWidth, posHeight, 0.f, 0.f ),
				Vec4( 0.f, posHeight * 0.5f, 0.f, 0.f ),
				Vec4( posWidth, posHeight * 0.5f, 0.f, 0.f )
			};
 			for( int i = 0; i < m_controllers.size(); i++ ) {
 				m_controllers[i]->DebugCameraInfoAt( pos[i] );
 			}
		}
		else if( m_debugMode == HELP_MODE ) {
			Strings helpStrings;
			helpStrings.push_back( "black is player debug position." );
			helpStrings.push_back( "white is debug pb position." );
			helpStrings.push_back( "red is intersect point A." );
			helpStrings.push_back( "green is intersect point B." );
			helpStrings.push_back( " is intersect point A." );
			DebugAddScreenLeftAlignStrings( 0.1f, 0.f, Rgba8( 255, 255, 255, 125 ), helpStrings );
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
	tempCamController->SetIndex( (int)m_controllers.size() - 1 );
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
	if( m_controllers.size() <= 1 ){ return; }
	else if( m_controllers.size() == 2 ) {
		ConstructVoronoiDiagramForTwoControllers( worldCameraBox, splitCheckBox );
	}
	else if( m_controllers.size() == 3 ) {
		ConstructVoronoiDiagramForThreeControllers( worldCameraBox, splitCheckBox );
	}
	else{
		ConstructVoronoiDiagramForMoreThanThreeControllers( worldCameraBox, splitCheckBox );
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
	std::vector<Vec2> pointsA;
	std::vector<Vec2> pointsB;
	std::pair<Vec2, Vec2> intersetPointsA = GetIntersectionPointOfLineAndAABB2( perpendicularLine, worldCameraBox );
	std::pair<Vec2, Vec2> intersetPointsB = GetIntersectionPointOfLineAndAABB2( perpendicularLine, worldCameraBox );

	pointsA.push_back( intersetPointsA.first );
	pointsA.push_back( intersetPointsA.second );
	pointsB.push_back( intersetPointsB.first );
	pointsB.push_back( intersetPointsB.second );

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
	m_playerDebugPositions.clear();
	m_debugPBLines.clear();
	m_debugIntersectPointsA.clear();
	m_debugIntersectPointsB.clear();
	m_debugIntersectPointsC.clear();

	Vec2 playerCameraPosA = m_controllers[0]->GetCameraPos();
	Vec2 playerCameraPosB = m_controllers[1]->GetCameraPos();
	Vec2 playerCameraPosC = m_controllers[2]->GetCameraPos();
	Vec2 pointA	= splitCheckBox.GetNearestPoint( playerCameraPosA );
	Vec2 pointB = splitCheckBox.GetNearestPoint( playerCameraPosB );
	Vec2 pointC = splitCheckBox.GetNearestPoint( playerCameraPosC );

// 	m_playerDebugPositions.push_back( pointA );
// 	m_playerDebugPositions.push_back( pointB );
// 	m_playerDebugPositions.push_back( pointC );
	m_playerDebugPosA = pointA;
	m_playerDebugPosB = pointB;
	m_playerDebugPosC = pointC;


	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( pointA, pointB );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( pointB, pointC );
	LineSegment2 PB_CA = GetPerpendicularBisectorOfTwoPoints( pointC, pointA );

	LineSegment2 debug_PB_AB = LineSegment2( PB_AB );
	LineSegment2 debug_PB_BC = LineSegment2( PB_BC );
	LineSegment2 debug_PB_CA = LineSegment2( PB_CA );
	float length = 10.f;
	debug_PB_AB.SetStartPos( debug_PB_AB.GetStartPos() - debug_PB_AB.GetNormalizedDirection() * length );
	debug_PB_AB.SetEndPos( debug_PB_AB.GetEndPos() + debug_PB_AB.GetNormalizedDirection() * length );
	debug_PB_BC.SetStartPos( debug_PB_BC.GetStartPos() - debug_PB_BC.GetNormalizedDirection() * length );
	debug_PB_BC.SetEndPos( debug_PB_BC.GetEndPos() + debug_PB_BC.GetNormalizedDirection() * length );
	debug_PB_CA.SetStartPos( debug_PB_CA.GetStartPos() - debug_PB_CA.GetNormalizedDirection() * length );
	debug_PB_CA.SetEndPos( debug_PB_CA.GetEndPos() + debug_PB_CA.GetNormalizedDirection() * length );

	m_debugPBLines.push_back( debug_PB_AB );
	m_debugPBLines.push_back( debug_PB_BC );
	m_debugPBLines.push_back( debug_PB_CA );

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
		if( intersectPointsA.size() == 0 || intersectPointsB.size() == 0 || intersectPointsC.size() == 0 ) {
			ERROR_RECOVERABLE( "intersectpoints should not be zero" );
		}
	}
	else {
		GetVoronoiPointsWithThreePointsNotCollinear( pointA, pointB, pointC, worldCameraBox, intersectPointsA, intersectPointsB, intersectPointsC );
		if( intersectPointsA.size() == 0 || intersectPointsB.size() == 0 || intersectPointsC.size() == 0 ) {
			//ERROR_RECOVERABLE( "intersectpoints should not be zero" );
		}
	}
// 	if( intersectPointsA.size() < 3 || intersectPointsB.size() < 3 || intersectPointsC.size() < 3 ) {
// 		ERROR_RECOVERABLE( "intersectpoints should not be zero" );
// 	}
// 	m_debugIntersectPointsA = std::vector<Vec2>( intersectPointsA );
// 	m_debugIntersectPointsB = std::vector<Vec2>( intersectPointsB );
// 	m_debugIntersectPointsC = std::vector<Vec2>( intersectPointsC );

	// temp disable for debug
	Polygon2 polygonA = Polygon2::MakeConvexFromPointCloud( intersectPointsA );
	Polygon2 polygonB = Polygon2::MakeConvexFromPointCloud( intersectPointsB );
	Polygon2 polygonC = Polygon2::MakeConvexFromPointCloud( intersectPointsC );

	m_controllers[0]->SetVoronoiPolygon( polygonA );
	m_controllers[1]->SetVoronoiPolygon( polygonB );
	m_controllers[2]->SetVoronoiPolygon( polygonC );
	m_controllers[0]->SetVoronoiOffset( Vec2::ZERO );
	m_controllers[1]->SetVoronoiOffset( Vec2::ZERO );
	m_controllers[2]->SetVoronoiOffset( Vec2::ZERO );

	m_DebugPolyA = polygonA;
	m_DebugPolyB = polygonB;
	m_DebugPolyC = polygonC;
	

// 	std::vector<std::vector<Vec2>> result;
// 	result.push_back( intersectPointsA );
// 	result.push_back( intersectPointsB );
// 	result.push_back( intersectPointsC );
}

void CameraSystem::ConstructVoronoiDiagramForMoreThanThreeControllers( AABB2 worldCameraBox, AABB2 splitCheckBox )
{
	ConstructVoronoiDiagramForThreeControllers( worldCameraBox, splitCheckBox );
	// step 1 find which cell is point in
	// for each cell
	// create PB and update intersect point
	// construct cell for new point
	// remove the old points
	const CameraController* currentController = nullptr;
	int currentCellNum = 3;
	for( int i = currentCellNum; i < m_controllers.size(); i++ ) {
		Vec2 playerCameraPosX = m_controllers[i]->GetCameraPos();
		Vec2 pointX	= splitCheckBox.GetNearestPoint( playerCameraPosX );

		for( int j = 0; j < currentCellNum; j++ ) {
			currentController = m_controllers[j];
			Polygon2 currentPolygon = currentController->GetVoronoiPoly();
			if( currentPolygon.IsPointInside( pointX ) ) {
				break;
			}
		}
		if( currentController == nullptr ) {
			ERROR_RECOVERABLE( "should have one polygon own the point" );
		}

		std::vector<Vec2> pointsX;
		std::stack<CameraController*> nextControllers;
		std::set<CameraController const*> checkedControllers;
		m_debugAfterDeletePoints.clear();
		m_debugIntersectPointsX.clear();
		while( true ) {
			Vec2 playerCameraPos = currentController->GetCameraPos();
			Vec2 point = splitCheckBox.GetNearestPoint( playerCameraPos );
			LineSegment2 PB_XD = GetPerpendicularBisectorOfTwoPoints( pointX, point );
			Polygon2 currentPolygon = currentController->GetVoronoiPoly();
			std::pair<Vec2, Vec2> intersectPoints = currentPolygon.GetIntersectPointWithStraightLine( PB_XD );
			Vec2 dirt = PB_XD.GetNormalizedDirection();
			LineSegment2 debugPB_XD = LineSegment2( PB_XD.GetStartPos() - dirt * 10.f, PB_XD.GetEndPos() + dirt * 10.f );
			m_debugPBLines.push_back( debugPB_XD );
			// check is point in aabb
			pointsX.push_back( intersectPoints.first );
			pointsX.push_back( intersectPoints.second );
			checkedControllers.insert( currentController );

			GetNextVoronoiPolygonControllerWithIntersectPoint( intersectPoints, currentController, nextControllers );

			if( nextControllers.empty() ){ break; }
			currentController = nextControllers.top();
			nextControllers.pop();
			if( checkedControllers.find( currentController ) != checkedControllers.end() ){ break; }
		}
			

		//reassign the box cornor
		Vec2 boxCornors[4];
		worldCameraBox.GetCornerPositions( boxCornors );
		for( int j = 0; j < currentCellNum; j++ ) {
			Polygon2 tempPoly = m_controllers[j]->GetVoronoiPoly();
			Vec2 playerCameraPos = m_controllers[j]->GetCameraPos();
			Vec2 point	= splitCheckBox.GetNearestPoint( playerCameraPos );
			std::vector<Vec2> tempPolyPoints = tempPoly.GetAllPoints();

			for( int k = 0; k < 4; k++ ) {
				for( int l = 0; l < tempPolyPoints.size(); l++ ) {
					if( tempPolyPoints[l] == boxCornors[k] && (GetDistanceSquared2D( pointX, boxCornors[k] ) <= (GetDistanceSquared2D( point, boxCornors[k] )))) {
						pointsX.push_back( tempPolyPoints[l] );
						tempPolyPoints.erase( tempPolyPoints.begin() + l );
						l--;
						break;
					}
				}
			}
		}
		m_debugIntersectPointsX.insert( m_debugIntersectPointsX.end(), pointsX.begin(), pointsX.end() ) ;
// 		
		if( pointsX.size() == 3 ) {
			int a = 0;
		}
 		Polygon2 polygonX = Polygon2::MakeConvexFromPointCloud( pointsX );
		m_DebugPolyD = polygonX;

		// get rid of points in polygonX
		for( int j = 0; j < currentCellNum; j++ ) {
			Polygon2 tempPoly = m_controllers[j]->GetVoronoiPoly();
			Vec2 playerCameraPos = m_controllers[j]->GetCameraPos();
			Vec2 point	= splitCheckBox.GetNearestPoint( playerCameraPos );

			std::vector<Vec2> tempPolyPoints = tempPoly.GetAllPoints();
			for( auto iter = tempPolyPoints.begin(); iter != tempPolyPoints.end(); ) {
				if( polygonX.IsPointInside( *iter ) ){
					iter = tempPolyPoints.erase( iter );
				}
				else {
					++iter;
				}
			}
			m_debugAfterDeletePoints.push_back( tempPolyPoints );
			if( tempPolyPoints.size() == 0 ) {
				int a = 0;
			}
			Polygon2 newPoly = Polygon2::MakeConvexFromPointCloud( tempPolyPoints );
			m_controllers[j]->SetVoronoiPolygon( newPoly );

		}	
		currentCellNum++;
	}
}

void CameraSystem::GetVoronoiPointsWithThreePointsInCollinearOrder( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, std::vector<Vec2>& pointsA, std::vector<Vec2>& pointsB, std::vector<Vec2>& pointsC )
{
	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( a, b );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( b, c );
	// abc same line in order a b c 
		// point order c a b 
	std::pair<Vec2,Vec2> intersectPointsA = GetIntersectionPointOfLineAndAABB2( PB_AB, worldCameraBox );
	std::pair<Vec2,Vec2> intersectPointsC = GetIntersectionPointOfLineAndAABB2( PB_BC, worldCameraBox );

	pointsA.push_back( intersectPointsA.first );
	pointsA.push_back( intersectPointsA.second );
	pointsC.push_back( intersectPointsC.first );
	pointsC.push_back( intersectPointsC.second );

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
	Vec2 centerABC = GetIntersectionPointOfTwoStraightLines( PB_AB, PB_AC );
	std::pair<Vec2,Vec2> PBIntersectWithBoxPoints_AB = GetIntersectionPointOfLineAndAABB2( PB_AB, worldCameraBox );
	std::pair<Vec2,Vec2> PBIntersectWithBoxPoints_AC = GetIntersectionPointOfLineAndAABB2( PB_AC, worldCameraBox );
	std::pair<Vec2,Vec2> PBIntersectWithBoxPoints_BC = GetIntersectionPointOfLineAndAABB2( PB_BC, worldCameraBox );
	 
// 	m_debugLineIntersectWithBoxPointsAB = PBIntersectWithBoxPoints_AB;
// 	m_debugLineIntersectWithBoxPointsBC = PBIntersectWithBoxPoints_BC;
// 	m_debugLineIntersectWithBoxPointsAC = PBIntersectWithBoxPoints_AC;

	pointsA = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( a, b, c, worldCameraBox, PBIntersectWithBoxPoints_AB, PBIntersectWithBoxPoints_AC );
	pointsB = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( b, a, c, worldCameraBox, PBIntersectWithBoxPoints_AB, PBIntersectWithBoxPoints_BC );
	pointsC = GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( c, a, b, worldCameraBox, PBIntersectWithBoxPoints_AC, PBIntersectWithBoxPoints_BC );

	if( worldCameraBox.IsPointInside( centerABC ) ) {
		pointsA.push_back( centerABC );
		pointsB.push_back( centerABC );
		pointsC.push_back( centerABC );
	}
}

std::vector<Vec2> CameraSystem::GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( Vec2 point, Vec2 helpPointA, Vec2 helpPointB, AABB2 worldCameraBox,std::pair<Vec2, Vec2> PBAPoints, std::pair<Vec2, Vec2> PBBPoints )
{
	// Need debug
	std::vector<Vec2> result;
	// get rid of far PB intersect points with box
	float PBAFirstPointDistSQToPoint			= GetDistanceSquared2D( PBAPoints.first, point );
	float PBAFirstPointDistSQToHelpPointB		= GetDistanceSquared2D( PBAPoints.first, helpPointB );
	float PBASecondPointDistSQToPoint			= GetDistanceSquared2D( PBAPoints.second, point );
	float PBASecondPointDistSQToHelpPointB		= GetDistanceSquared2D( PBAPoints.second, helpPointB );

	float PBBFirstPointDistSQToPoint			= GetDistanceSquared2D( PBBPoints.first, point );
	float PBBFirstPointDistSQToHelpPointA		= GetDistanceSquared2D( PBBPoints.first, helpPointA );
	float PBBSecondPointDistSQToPoint			= GetDistanceSquared2D( PBBPoints.second, point  );
	float PBBSecondPointDistSQToHelpPointA		= GetDistanceSquared2D( PBBPoints.second, helpPointA );
	if( PBAFirstPointDistSQToPoint < PBAFirstPointDistSQToHelpPointB ) {
		result.push_back( PBAPoints.first );
	}
	else {
		result.push_back( PBAPoints.second );
	}
	if( PBBFirstPointDistSQToPoint < PBBFirstPointDistSQToHelpPointA ) {
		result.push_back( PBBPoints.first );
	}
	else {
		result.push_back( PBBPoints.second );
	}

	Vec2 worldCameraBoxCorners[4];
	worldCameraBox.GetCornerPositions( worldCameraBoxCorners );
	for( int i = 0; i < 4; i++ ) {
		float distToPoint	= GetDistance2D( point, worldCameraBoxCorners[i] );
		float distToPointA	= GetDistance2D( helpPointA, worldCameraBoxCorners[i] );
		float distToPointB	= GetDistance2D( helpPointB, worldCameraBoxCorners[i] );
		if( distToPoint <= distToPointA && distToPoint <= distToPointB ) {
			result.push_back( worldCameraBoxCorners[i] );
		}
	}


// 	for( int i = 0; i < PBAPoints.size(); i++ ) {
// 		Vec2 tempPBPoint = PBAPoints[i];
// 		float distToPoint = GetDistance2D( point, tempPBPoint );
// 		float distToHelpPointA = GetDistance2D( tempPBPoint, helpPointA );
// 		float distToHelpPointB = GetDistance2D( tempPBPoint, helpPointB );
// 		if( distToPoint <= distToHelpPointA && distToPoint <= distToHelpPointB ) {
// 			result.push_back( tempPBPoint );
// 		}
// 	}
// 
// 	for( int i = 0; i < PBBPoints.size(); i++ ) {
// 		Vec2 tempPBPoint = PBBPoints[i];
// 		float distToPoint = GetDistance2D( point, tempPBPoint );
// 		float distToHelpPointA = GetDistance2D( tempPBPoint, helpPointA );
// 		float distToHelpPointB = GetDistance2D( tempPBPoint, helpPointB );
// 		if( distToPoint <= distToHelpPointA && distToPoint <= distToHelpPointB ) {
// 			result.push_back( tempPBPoint );
// 		}
// 	}
// 	// Get nearest AABB point
	return result;
}

void CameraSystem::GetNextVoronoiPolygonControllerWithIntersectPoint( std::pair<Vec2, Vec2> intersectPoints, const CameraController* currentController, std::stack<CameraController*>& nextControllers )
{
	// might have more that one adjacent edge???
	Polygon2 currentPoly = currentController->GetVoronoiPoly();
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i] == currentController ) { continue; }
		Polygon2 testPoly = m_controllers[i]->GetVoronoiPoly();
		LineSegment2 sharedEdge;
		if( GetSharedEdgeOfTwoPolygon( sharedEdge, currentPoly, testPoly ) ){
			if( sharedEdge.IsPointMostlyInEdge( intersectPoints.first ) || sharedEdge.IsPointMostlyInEdge( intersectPoints.second ) ) {
				nextControllers.push( m_controllers[i] );
			}
		}
	}
}

 bool CameraSystem::GetSharedEdgeOfTwoPolygon( LineSegment2& sharedLine, Polygon2 polyA, Polygon2 polyB )
 {
	 for( int i = 0; i < polyA.GetEdgeCount(); i++ ) {
		 for( int j = 0; j < polyB.GetEdgeCount(); j++ ) {
			 LineSegment2 lineA = polyA.GetEdgeInWorld( i );
			 LineSegment2 lineB = polyB.GetEdgeInWorld( j );
			 if( IsVec2MostlyEqual( lineA.GetStartPos(), lineB.GetStartPos() ) && IsVec2MostlyEqual( lineA.GetEndPos(), lineB.GetEndPos() ) ) {
				sharedLine = lineA;
				return true;
			 }
			 else if( IsVec2MostlyEqual( lineA.GetStartPos(), lineB.GetEndPos() ) && IsVec2MostlyEqual( lineA.GetEndPos(), lineB.GetStartPos() ) ) {
				 sharedLine = lineA;
				 return true;
			 }
		 }
	 }
	 return false;	
 }

