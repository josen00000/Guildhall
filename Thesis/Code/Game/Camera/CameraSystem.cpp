#include "CameraSystem.hpp"
#include "Game/Player.hpp"
#include "Game/Game.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


//#define DEBUG_MODE

//-------------------------------------------------------------------------------------------------------
extern Game*			g_theGame;
extern Camera*			g_gameCamera;
extern RenderContext*	g_theRenderer;

//-------------------------------------------------------------------------------------------------------
void CameraSystem::Startup()
{
	m_controllers.reserve( 4 );
	m_rng = new RandomNumberGenerator( 1 );
	m_noSplitCamera = g_gameCamera;
	m_noSplitCamera->EnableClearColor( Rgba8::DARK_GRAY );
	m_multipleCameraShader = g_theRenderer->GetOrCreateShader( "data/Shader/multipleCamera.hlsl" );
}

//-------------------------------------------------------------------------------------------------------
void CameraSystem::Shutdown()
{
	delete m_rng;
	m_rng = nullptr;
	DestroyAllControllers();
}

//-------------------------------------------------------------------------------------------------------
void CameraSystem::BeginFrame()
{
	m_postVoronoiIteration = 0;
	for( int i =0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->BeginFrame();
	}
}

//-------------------------------------------------------------------------------------------------------
void CameraSystem::EndFrame()
{
	// game code. Should not exist in camera system
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

//-------------------------------------------------------------------------------------------------------
void CameraSystem::Update( float deltaSeconds )
{
	UpdateControllers( deltaSeconds );
	UpdateSplitScreenEffects();
	UpdateControllerCameras();
#ifdef DEBUG_MODE
	UpdateDebugInfo();
#endif // DEBUG_MODE
	AABB2 worldCameraBox = m_noSplitCamera->GetWorldBox();
}

void CameraSystem::Render()
{
	if( m_isdebug ) {
		g_theRenderer->BeginCamera( m_noSplitCamera );
		g_theGame->RenderGame();
		DebugRender();
		g_theRenderer->EndCamera();
	}
	else {
		if( m_splitScreenState == NO_SPLIT_SCREEN ) {
			if( m_noSplitCamera == nullptr ) {
				m_noSplitCamera = g_theGame->m_gameCamera;
			}
			if( m_controllers.size() == 1 ) {
				m_noSplitCamera->m_transform = m_controllers[0]->m_camera->m_transform;
			}
			g_theRenderer->BeginCamera( m_noSplitCamera );
			g_theGame->RenderGame();
			g_theRenderer->EndCamera();
		}
		else if( m_splitScreenState == VORONOI_SPLIT ) {
			for( int i = 0; i < m_controllers.size(); i++ ) {
				if( !m_controllers[i]->GetNeedRenderWhenMerged() ){ continue; }
				m_controllers[i]->Render();
			}
			g_theRenderer->BeginCamera( m_noSplitCamera );
			g_theRenderer->BindShader( m_multipleCameraShader );

			for( int i = 0; i < m_controllers.size(); i++ ) {
				if( !m_controllers[i]->GetNeedRenderWhenMerged() ){ continue; }
				g_theRenderer->SetDiffuseTexture( m_controllers[i]->GetColorTarget() );
				g_theRenderer->SetMaterialTexture( m_controllers[i]->GetStencilTexture() );
				g_theRenderer->SetOffsetBuffer( m_controllers[i]->GetOffsetBuffer(), 0 );
				g_theRenderer->m_context->Draw( 3, 0 );
			}
			g_theRenderer->EndCamera();

			for( int i = 0; i < m_controllers.size(); i++ ) {
				if( !m_controllers[i]->GetNeedRenderWhenMerged() ) { continue; }
				m_controllers[i]->ReleaseRenderTarget();
			}
		
		}
		else if( m_splitScreenState == AXIS_ALIGNED_SPLIT ) {
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

//-------------------------------------------------------------------------------------------------------
void CameraSystem::UpdateSplitScreenEffects( )
{
	switch( m_splitScreenState )
	{
		case NO_SPLIT_SCREEN: {
			UpdateNoSplitScreenEffect( );
		}
		break;
		case VORONOI_SPLIT: {
			UpdatePreVoronoiSplitScreenEffect();
			UpdateVoronoiSplitScreenEffect();
			UpdatePostVoronoiSplitScreenEffects();
		}
		break;
		case AXIS_ALIGNED_SPLIT:
			return;
		case NUM_OF_SPLIT_SCREEN_STATE:
			return;
	}
}

//-------------------------------------------------------------------------------------------------------
void CameraSystem::UpdateNoSplitScreenEffect( )
{
	if( m_controllers.size() == 0 ){ return; }

	std::vector<CameraController*> notInsideControllers;
	switch( m_noSplitScreenStrat )
	{
		case NO_STRAT:
			break;
		case ZOOM_TO_FIT: {
			GetNotInsideControllers( notInsideControllers, m_notInsideOutPaddingLength );
			ZoomCameraToFitPlayers( notInsideControllers );
			break;
		}
		case KILL_AND_TELEPORT: {
			GetNotInsideControllers( notInsideControllers, m_notInsideInPaddingLength );
			KillAndTeleportPlayers( notInsideControllers );
		}
		break;
		case COMBINATION_ZOOM_AND_KILL: {
			GetNotInsideControllers( notInsideControllers, m_notInsideOutPaddingLength );
			ZoomCameraToFitPlayers( notInsideControllers );
			GetNotInsideControllers( notInsideControllers, m_notInsideInPaddingLength );
			KillAndTeleportPlayers( notInsideControllers );
		}
	}
}

//-------------------------------------------------------------------------------------------------------
void CameraSystem::UpdatePreVoronoiSplitScreenEffect()
{
	// initialize
	for( int i = 0; i < MAX_CAMERA_CONTROLLER_NUM; i++ ) {
		if( i < m_controllers.size() ) {
			m_controllers[i]->SetNeedRenderWhenMerged( true );
		}
		for( int j = i; j < 5; j++ ) {
			m_controllersSplitToMergeBlendCoeffs[i][j] =  0.f ;
			m_controllersSplitToMergeBlendCoeffs[j][i] =  0.f ;
		}
	}

	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( !m_controllers[i]->GetAllowMerge() ){ continue; }
		Vec2 controllerAPos = m_controllers[i]->GetCameraPos();

		for( int j = i + 1; j < m_controllers.size(); j++ ) {
			Vec2 controllerBPos = m_controllers[j]->GetCameraPos();
			float dist = GetDistance2D( controllerAPos, controllerBPos );
			if( dist < m_totalMergedDist ) {
				m_controllersSplitToMergeBlendCoeffs[i][j] = 1.f; 
				m_controllersSplitToMergeBlendCoeffs[j][i] = 1.f;
			}
			else if( dist < m_totalSplitDist ) {
 				if( !m_controllers[j]->GetAllowBlend() ){ continue; }
				float splitToMergeBlendCoeff = RangeMapFloat( m_totalSplitDist, m_totalMergedDist, 0.f, 1.f,  dist );
				m_controllersSplitToMergeBlendCoeffs[i][j] = splitToMergeBlendCoeff;
				m_controllersSplitToMergeBlendCoeffs[j][i] = splitToMergeBlendCoeff;
			}
		}
	}

// 	for( int i = 0; i < MAX_CAMERA_CONTROLLER_NUM; i++ ) {
// 		if( IsControllerNeedRenderWhenMerged( i ) && i < m_controllers.size()  ) {
// 			m_controllers[i]->SetNeedRenderWhenMerged( true );
// 		}
// 	}
}

void CameraSystem::UpdateVoronoiSplitScreenEffect()
{
	AABB2 worldCameraBox = GetWorldCameraBox();
	AABB2 voronoiBox	= GetVoronoiBox();
	Vec2 pos = GetAverageCameraPosition();
	m_noSplitCamera->SetPosition2D( pos );
	
	ComputeAndUpdateOriginalVoronoiAnchorPoints( worldCameraBox, voronoiBox, m_isVoronoiInitialized );
	if( !m_isVoronoiInitialized ) {
		ConstructVoronoiDiagramForControllers( worldCameraBox, voronoiBox, INITIALIZE_POLY );
		m_isVoronoiInitialized = true;
	}
	else {
		ConstructVoronoiDiagramForControllers( worldCameraBox, voronoiBox, ORIGINAL_POLY );
	}
}


void CameraSystem::UpdatePostVoronoiSplitScreenEffects()
{
	// update target voronoi anchor point
	// Compute current anchorpoint
	// reconstruct voronoi diagram
	
	switch( m_postVoronoiSetting )
	{
		case NO_POST_EFFECT: {
			for( int i = 0; i < m_controllers.size(); i++ ) {
				m_controllers[i]->SetVoronoiPolygon( m_controllers[i]->GetOriginalVoronoiPolygon() );
			}
		}
		break;
		case BALANCED_VORONOI_DIAGRAM: {
			if( m_controllers.size() <= 2 ) {
				for( int i = 0; i < m_controllers.size(); i++ ) {
					m_controllers[i]->SetVoronoiPolygon( m_controllers[i]->GetOriginalVoronoiPolygon() );
				}
			}
			AABB2 worldCameraBox = GetWorldCameraBox();
			AABB2 voronoiBox = GetVoronoiBox();
			InitializeVoronoiTargetPointWithVoronoiOriginalPoint();
			while( m_postVoronoiIteration < m_targetPostVoronoiIteration ) {
				ComputeAndSetBalancedVoronoiTargetAnchorPoints( worldCameraBox, voronoiBox );
				ReconstructVoronoiDiagramWithTargetAnchorPoints();
				m_postVoronoiIteration++;
			}
		}
		break;
		case HEALTH_BASED_VORONOI_DIAGRAM:{
			//ReContructHealthBasedVoronoiDiagram();
		}
	}
	UpdateVoronoiPolyAndOffset();
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
	g_theRenderer->SetMaterialTexture( nullptr );

	if( GetDoesDebugAnchorPoints()) {
		float thickness = 0.1f;
		for( int i = 0; i < m_controllers.size(); i++ ) {
			g_theRenderer->DrawCircle( m_controllers[i]->GetVoronoiAnchorPointPos(), 0.1f, thickness, Rgba8::RED );
			g_theRenderer->DrawCircle( m_controllers[i]->GetTargetVoronoiAnchorPointPos(), 0.2f, thickness, Rgba8::GREEN );
			g_theRenderer->DrawCircle( m_controllers[i]->GetOriginalVoronoiAnchorPointPos(), 0.3f, thickness, Rgba8::BLUE );
		}
	}

	g_theRenderer->DrawCircle( m_debugMergedPoint, 0.1f, 0.1f, Rgba8::RED );
	g_theRenderer->DrawCircle( m_debugMergedPoint, 0.2f, 0.1f, Rgba8::BLACK );
	DebugRenderControllers();
}

void CameraSystem::DebugRenderControllers()
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->DebugRender();
	}
}

void CameraSystem::CheckIsControllerVaild( int controllerIndex ) const
{
	if( controllerIndex >= m_controllers.size() ) {
		ERROR_RECOVERABLE( "Controller does not exist" );
	}
}

float CameraSystem::GetTotalFactor() const
{
	float totalFactor = 0.f;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		totalFactor += m_controllers[i]->GetCurrentMultipleFactor();
	}
	return totalFactor;
}

float CameraSystem::GetForwardVelocityCameraFrameDist() const
{
	if( m_controllers.size() == 0 ){ return 5.f; }
	else {
		return m_controllers[0]->GetForwardVelocityCameraFrameDist();
	}
}

float CameraSystem::GetAimCameraFrameDist() const
{
	if( m_controllers.size() == 0 ) { return 5.f; }
	else {
		return m_controllers[0]->GetAimCameraFrameDist();
	}
}

float CameraSystem::GetControllerForwardVelocityCameraFrameRatio( int controllerIndex ) const
{
	CheckIsControllerVaild( controllerIndex );
	return	m_controllers[controllerIndex]->GetForwardVelocityCameraFrameRatio();
}

float CameraSystem::GetControllerAimCameraFrameRatio( int controllerIndex ) const
{
	CheckIsControllerVaild( controllerIndex );
	return	m_controllers[controllerIndex]->GetAimCameraFrameRatio();
}

float CameraSystem::GetControllerCueCameraFrameRatio( int controllerIndex ) const
{
	CheckIsControllerVaild( controllerIndex );
	return	m_controllers[controllerIndex]->GetCueCameraFrameRatio();
}

float CameraSystem::GetPositionalShakeMaxDist() const
{
	if( m_controllers.size() == 0 ){ return 5.f;}
	else {
		return m_controllers[0]->m_maxShakePosDist;
	}
}

float CameraSystem::GetRotationalShakeMaxDeg() const
{
	if( m_controllers.size() == 0 ) { return 5.f; }
	else {
		return m_controllers[0]->m_maxShakeRotDeg;
	}
}

Vec2 CameraSystem::GetCameraWindowSize() const
{
	if( m_controllers.size() == 0 ){ return Vec2::ZERO; }
	else {
		return m_controllers[0]->GetCameraWindowSize();
	}
}

bool CameraSystem::GetDoesDebugCameraWindow() const
{
	return ( m_debugFlags & DEBUG_CAMERA_WINDOW_BIT ) == DEBUG_CAMERA_WINDOW_BIT;
}

bool CameraSystem::GetDoesDebugAnchorPoints() const
{
	return ( m_debugFlags & DEBUG_VORONOI_ANCHOR_POINT_BIT ) == DEBUG_VORONOI_ANCHOR_POINT_BIT;
}

bool CameraSystem::GetDoesDebugCameraPos() const
{
	return ( m_debugFlags & DEBUG_CAMERA_POSITION_BIT ) == DEBUG_CAMERA_POSITION_BIT;
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
	case NUM_OF_CAMERA_SNAPPING_STATE:
		return result +std::string( "Num of camera snapping" );
	}
	return " Error state";
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
	switch( m_debugFlags )
	{
	case CONTROLLER_INFO:
		return result + std::string( "Controller Info" );
	case VORONOI_INFO:
		return result + std::string( "voronoi Info" );
	case HELP_MODE:
		return result + std::string( "Help Mode" );
	}
	return result + "Error state.";
}

std::string CameraSystem::GetPostVoronoiSettingText() const {
	std::string result = "Post Voronoi Setting : ";
	switch( m_postVoronoiSetting )
	{
	case NO_POST_EFFECT:
		return result + "No post voronoi effect";
	case BALANCED_VORONOI_DIAGRAM:
		return result + "Balanced voronoi diagram";
	case HEALTH_BASED_VORONOI_DIAGRAM:
		return result + "Health base voronoi diagram";
	}
	return result + "Error state.";
}

std::string CameraSystem::GetVoronoiAreaCheckStateText() const
{
	std::string result = "Voronoi area check state : ";
	switch( m_voronoiAreaCheckState )
	{
	case POLYGON_AREA:
		return result + "polygon area";
	case INCIRCLE_RADIUS:
		return result + "incircle area";
	}
	return result + "Error state.";
}

Vec2 CameraSystem::GetAverageCameraPosition() const
{
	Vec2 totalPos = Vec2::ZERO;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		totalPos += tempController->GetCameraPos();
	}
	totalPos /= (float)m_controllers.size();
	return totalPos;
}

AABB2 CameraSystem::GetWorldCameraBox() const
{
	Vec2 boxCenter = GetAverageCameraPosition();
	Vec2 boxDimension = g_gameCamera->Get2DDimension();
	AABB2 worldCameraBox = AABB2( boxCenter, boxDimension.x, boxDimension.y ); 
	return worldCameraBox;
}

AABB2 CameraSystem::GetVoronoiBox() const
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

CameraController* CameraSystem::GetCameraControllerWithIndex( int index )
{
	if( index >= m_controllers.size() ) {
		return nullptr;
	}
	return m_controllers[index];
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

void CameraSystem::SetAllowMerge( bool allowMerge )
{
	m_doesAllowVoronoiMerge = allowMerge;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->SetAllowMerge( m_doesAllowVoronoiMerge );
	}
}

void CameraSystem::SetAllowBlend( bool allowBlend )
{
	m_doesAllowVoronoiBlend = allowBlend;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->SetAllowBlend( m_doesAllowVoronoiBlend );
	}
}

void CameraSystem::SetDoesUseCameraFrame( bool doesUseFrame )
{
	m_doesUseCameraFrame = doesUseFrame;
// 	for( int i = 0; i < m_controllers.size(); i++ ) {
// 		m_controllers[i]->SetUseCameraFrame( doesUseFrame );
// 	}
}

void CameraSystem::SetCameraWindowState( CameraWindowState newState )
{
	m_cameraWindowState = newState;
}

void CameraSystem::SetCameraSnappingState( CameraSnappingState newState )
{
	m_cameraSnappingState = newState;
}

void CameraSystem::SetSplitScreenState( SplitScreenState newState )
{
	m_splitScreenState = newState;
	m_isVoronoiInitialized = false;
}

void CameraSystem::SetNoSplitScreenStrat( NoSplitScreenStrat newStrat )
{
	m_noSplitScreenStrat = newStrat;
}

void CameraSystem::SetDebugMode( DebugMode newMode )
{
	m_debugMode = newMode;
}

void CameraSystem::SetPostVoronoiSetting( PostVoronoiSetting newSetting ) {
	m_postVoronoiSetting = newSetting;
}

void CameraSystem::SetVoronoiAreaCheckState( VoronoiAreaCheckState voronoiAreaCheckState )
{
	m_voronoiAreaCheckState = voronoiAreaCheckState;
}

void CameraSystem::SetMap( Map* map )
{
	m_map = map;
}

void CameraSystem::SetCameraWindowSize( Vec2 dimension )
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->SetCameraWindowSize( dimension );
	}
}

void CameraSystem::AddCameraShake( int index, float shakeTrauma )
{
	if( index < m_controllers.size() ) {
		m_controllers[index]->AddTrauma( shakeTrauma );
	}
}


void CameraSystem::SetForwardVelocityFrameDistance( float dist )
{
	for( CameraController* controller : m_controllers ) {
		controller->SetForwardVelocityFocusDist( dist );
	}
}

void CameraSystem::SetAimFocusDistance( float dist )
{
	for( CameraController* controller : m_controllers ) {
		controller->SetAimFocusDist( dist );
	}
}

void CameraSystem::SetControllerUseCameraFrame( int controllerIndex, bool useCameraFrame )
{
	m_controllers[controllerIndex]->SetUseCameraFrame( useCameraFrame );
}

void CameraSystem::SetControllerAimFocusDistance( int controllerindex, float dist )
{
	m_controllers[controllerindex]->SetAimFocusDist( dist );
}

void CameraSystem::SetControllerForwardVelocityFrameRatio( int controllerIndex, float ratio )
{
	if( controllerIndex >= m_controllers.size() ){
		//ERROR_RECOVERABLE( "Can not assign value to controller not exist!");
		return;
	}
	m_controllers[controllerIndex]->SetForwardVelocityFocusRatio( ratio );
}

void CameraSystem::SetControllerAimFocusRatio( int controllerIndex, float ratio )
{
	if( controllerIndex >= m_controllers.size() ) {
		//ERROR_RECOVERABLE( "Can not assign value to controller not exist!" );
		return;
	}
	m_controllers[controllerIndex]->SetAimFocusRatio( ratio );
}

void CameraSystem::SetControllerCueFocusRatio( int controllerIndex, float ratio )
{
	if( controllerIndex >= m_controllers.size() ) {
		//ERROR_RECOVERABLE( "Can not assign value to controller not exist!" );
		return;
	}
	m_controllers[controllerIndex]->SetCueFocusRatio( ratio );
}

void CameraSystem::SetControllerEdgeThickness( int controllerIndex, float thickness )
{
	m_controllers[controllerIndex]->SetMaxEdgeThickness( thickness );
}

void CameraSystem::SetControllerMaxAsymptoticValue( int controllerIndex, float maxValue )
{
	m_controllers[controllerIndex]->SetMaxAsymptoticValue( maxValue );
}

void CameraSystem::SetControllerMinAsymptoticValue( int controllerIndex, float minValue )
{
	m_controllers[controllerIndex]->SetMinAsymptoticValue( minValue );
}

void CameraSystem::SetControllerMaxDeltaDist( int controllerIndex, float maxDist )
{
	m_controllers[controllerIndex]->SetMaxDeltaDistance( maxDist );
}

void CameraSystem::SetPositionalShakeMaxDist( float maxDist )
{
	for( CameraController* controller: m_controllers ) {
		controller->SetPositionalShakeMaxDist( maxDist );
	}
}

void CameraSystem::SetRotationalShakeMaxDeg( float maxDeg )
{
	for( CameraController* controller: m_controllers ) {
		controller->SetRotationalShakeMaxDeg( maxDeg );
	}
}

void CameraSystem::SetPostVoronoiIterationNum( int num )
{
	m_targetPostVoronoiIteration = num;
}

void CameraSystem::SetDoesDebugCameraWindow( bool doesDebugCameraWindow )
{
	if( doesDebugCameraWindow ) {
		m_debugFlags = m_debugFlags | DEBUG_CAMERA_WINDOW_BIT;
	}
	else {
		m_debugFlags = m_debugFlags & ~DEBUG_CAMERA_WINDOW_BIT;
	}
}

void CameraSystem::SetDoesDebugCameraPosition( bool doesDebugCameraPosition )
{
	if( doesDebugCameraPosition ) {
		m_debugFlags = m_debugFlags | DEBUG_CAMERA_POSITION_BIT;
	}
	else {
		m_debugFlags = m_debugFlags & ~DEBUG_CAMERA_POSITION_BIT;
	}
}

void CameraSystem::SetDoesDebugAnchorPoints( bool doesDebugAnchorPoints )
{
	if( doesDebugAnchorPoints ) {
		m_debugFlags = m_debugFlags | DEBUG_VORONOI_ANCHOR_POINT_BIT;
	}
	else {
		m_debugFlags = m_debugFlags & ~DEBUG_VORONOI_ANCHOR_POINT_BIT;
	}
}

void CameraSystem::UpdateDebugInfo()
{
	std::string debugText				= "Press F1 to enable debug mode.";
	std::string windowStateText			= "Press F2 to change: " + GetCameraWindowStateText();
	std::string snappingStateText		= "Press F3 to change: " + GetCameraSnappingStateText(); 
	std::string splitStateText			= "Press F6 to change: " + GetSplitScreenStateText();
	std::string splitStratText			= "Press F7 to change: ";
	std::string postVoronoiSettingText	= "Press F8 to change: " + GetPostVoronoiSettingText();
	std::string voronoiAreaCheckText	= "Press 2 to change: " + GetVoronoiAreaCheckStateText(); 
	std::string debugModeText			= "Press F9 to change: " + GetDebugModeText();

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
	//debugInfos.push_back( debugText );
	//debugInfos.push_back( windowStateText );
	//debugInfos.push_back( snappingStateText );
	//debugInfos.push_back( shakeStateText );
	//debugInfos.push_back( frameStateText );
	//debugInfos.push_back( splitStateText );
	//debugInfos.push_back( splitStratText );
	//debugInfos.push_back( postVoronoiSettingText );
	//debugInfos.push_back( debugModeText ); 
	//debugInfos.push_back( voronoiAreaCheckText );

	if( m_noSplitCamera ) {
		float height = m_noSplitCamera->GetCameraHeight();
		std::string heightText = std::string( " Camera height is " + std::to_string( height ));
		debugInfos.push_back( heightText );
	}

	if( m_isdebug ) {
		DebugAddScreenLeftAlignStrings( 0.98f, 0.f, Rgba8( 255, 255, 255, 125), debugInfos );
	}

	if( m_isdebug ) {
		if( m_debugMode == CONTROLLER_INFO || m_debugMode == VORONOI_INFO ) {
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
	tempCamController->SetCameraWindowSize( Vec2( DEFAULT_CAMERA_WINDOW_WIDTH, DEFAULT_CAMERA_WINDOW_HEIGHT ) );
	tempCamController->SetAllowMerge( m_doesAllowVoronoiMerge );
	tempCamera->SetPosition( player->GetPosition() );
	m_controllers.push_back( tempCamController );
	tempCamController->SetIndex( (int)m_controllers.size() - 1 );
	float smoothTime = 2.f;
	if( m_controllers.size() == 1 ){
		smoothTime = 0.1f;
	}
	UpdateControllerMultipleFactor( smoothTime );
	m_isVoronoiInitialized = false;
}

void CameraSystem::DestroyAllControllers()
{
	for( CameraController* controller: m_controllers ) {
		delete controller;	
	}
	m_controllers.clear();
}

void CameraSystem::PrepareRemoveAndDestroyController( Player const* player, float smoothTime )
{
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

//-------------------------------------------------------------------------------------------------------
void CameraSystem::ZoomCameraToFitPlayers( std::vector<CameraController*>& notInsideControllers )
{
	static int zoomRecoverWaitFrames = 0;
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
		tempNewHalfHeight += 2.f;
		if( newHalfHeight < tempNewHalfHeight ) {
			newHalfHeight = tempNewHalfHeight;
		}
	}
	if( newHalfHeight * 2 > m_maxCameraHeight ) {
		return m_maxCameraHeight;
	}
	return ( newHalfHeight * 2 );
}

void CameraSystem::SetDynamicAxisAlignedSplitScreenMultipleFactors( float smoothTime, float factor1, float factor2 )
{
	//float smoothTime = 0.1f;
	m_controllers[0]->SetMultipleCameraStableFactorNotStableUntil( smoothTime, factor1 );
	m_controllers[1]->SetMultipleCameraStableFactorNotStableUntil( smoothTime, factor2 );
}

void CameraSystem::ConstructVoronoiDiagramForControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type )
{
	if( m_controllers.size() == 0 ){ return; }
	else if( m_controllers.size() == 1 ){
		ConvexPoly2 worldCameraBoxPoly = ConvexPoly2::MakeConvexPolyFromAABB2( worldCameraBox );
		m_controllers[0]->SetOriginalVoronoiPolygon( worldCameraBoxPoly );
	}
	else if( m_controllers.size() == 2 ) {
		ConstructVoronoiDiagramForTwoControllers( worldCameraBox, voronoiBox );
	}
	else if( m_controllers.size() == 3 ) {
		ConstructVoronoiDiagramForThreeControllers( worldCameraBox, voronoiBox,type );
	}
	else{
		ConstructVoronoiDiagramForMoreThanThreeControllers( worldCameraBox, voronoiBox,type );
	}
}

void CameraSystem::ConstructVoronoiDiagramForTwoControllers( AABB2 worldCameraBox, AABB2 voronoiBox )
{
	Vec2 player1CameraPos = m_controllers[0]->m_cameraPos;
	Vec2 player2CameraPos = m_controllers[1]->m_cameraPos;
	Vec2 dispAB		= player2CameraPos - player1CameraPos;
	Vec2 centerPos	= (player1CameraPos + player2CameraPos) / 2;
	Vec2 dirtAB		= dispAB.GetNormalized();
	Vec2 dirtPerpendicularAB = Vec2( -dirtAB.y, dirtAB.x );
	LineSegment2 perpendicularLine = LineSegment2( centerPos, centerPos + dirtPerpendicularAB * 5.f );
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
	ConvexPoly2 polygonA = ConvexPoly2::MakeConvexPolyFromPointCloud( pointsA );
	ConvexPoly2 polygonB = ConvexPoly2::MakeConvexPolyFromPointCloud( pointsB );

	m_controllers[0]->SetOriginalVoronoiPolygon( polygonA );
	m_controllers[1]->SetOriginalVoronoiPolygon( polygonB );
}

void CameraSystem::ConstructVoronoiDiagramForThreeControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type )
{
	m_controllerVoronoiEdges.clear();
	Vec2 pointA;
	Vec2 pointB;
	Vec2 pointC;
	switch( type )
	{
	case ORIGINAL_POLY:
		pointA = m_controllers[0]->GetOriginalVoronoiAnchorPointPos();
		pointB = m_controllers[1]->GetOriginalVoronoiAnchorPointPos();
		pointC = m_controllers[2]->GetOriginalVoronoiAnchorPointPos();
		break;
	case CURRENT_POLY:
		pointA = m_controllers[0]->GetVoronoiAnchorPointPos();
		pointB = m_controllers[1]->GetVoronoiAnchorPointPos();
		pointC = m_controllers[2]->GetVoronoiAnchorPointPos();
		break;
	case INITIALIZE_POLY:
		pointA = m_controllers[0]->GetOriginalVoronoiAnchorPointPos();
		pointB = m_controllers[1]->GetOriginalVoronoiAnchorPointPos();
		pointC = m_controllers[2]->GetOriginalVoronoiAnchorPointPos();
	default:
		break;
	}
// 	if( pointA == pointB || pointB == pointC || pointA == pointC ) {
// 		int a  =0;
// 	}

	LineSegment2 PB_AB = GetPerpendicularBisectorOfTwoPoints( pointA, pointB );
	LineSegment2 PB_BC = GetPerpendicularBisectorOfTwoPoints( pointB, pointC );
	LineSegment2 PB_CA = GetPerpendicularBisectorOfTwoPoints( pointC, pointA );

	// Check if abc colinear
	LineSegment2 AB = LineSegment2( pointA, pointB );
	std::vector<Vec2> intersectPointsA;
	std::vector<Vec2> intersectPointsB;
	std::vector<Vec2> intersectPointsC;
	ConvexHull2 hullA;
	ConvexHull2 hullB;
	ConvexHull2 hullC;
		
	if( AB.IsPointMostlyInStraightLine( pointC ) ) {
		// abc colinear
		Vec2 disp_AB = pointB - pointA;
		Vec2 disp_AC = pointC - pointA;
		float DP_AB_AC = DotProduct2D( disp_AB, disp_AC );
		float DP_AB_AB = DotProduct2D( disp_AB, disp_AB );
		if( DP_AB_AC < 0 ) {
			// order is CAB
			ConstructVoronoiHullsWithThreePointsInCollinearOrder( pointC, pointA, pointB, worldCameraBox, hullC, hullA, hullB );
		}
		else if( DP_AB_AC < DP_AB_AB ){
			// order is acb
			ConstructVoronoiHullsWithThreePointsInCollinearOrder( pointA, pointC, pointB, worldCameraBox, hullA, hullC, hullB );
		}
		else{
			// order is abc
			ConstructVoronoiHullsWithThreePointsInCollinearOrder( pointA, pointB, pointC, worldCameraBox, hullA, hullB, hullC );
		}
	}
	else {
		ConstructVoronoiHullsWithThreePointsNotCollinear( pointA, pointB, pointC, worldCameraBox, hullA, hullB, hullC );
	}
	m_controllerVoronoiEdges.push_back( hullA );
	m_controllerVoronoiEdges.push_back( hullB );
	m_controllerVoronoiEdges.push_back( hullC );
	hullA.AddAABB2Planes( worldCameraBox );
	hullB.AddAABB2Planes( worldCameraBox );
	hullC.AddAABB2Planes( worldCameraBox );
	m_controllers[0]->SetVoronoiHullAndUpdateVoronoiPoly( hullA, type );
	m_controllers[1]->SetVoronoiHullAndUpdateVoronoiPoly( hullB, type );
	m_controllers[2]->SetVoronoiHullAndUpdateVoronoiPoly( hullC, type );
}

void CameraSystem::ConstructVoronoiDiagramForMoreThanThreeControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type )
{
	// step 1 find which cell is point in
	// for each cell
	// create PB and update intersect point
	// construct cell for new point
	// remove the old points

	ConstructVoronoiDiagramForThreeControllers( worldCameraBox, voronoiBox, type );
	ConvexHull2 worldCameraHull = ConvexHull2( worldCameraBox );

	
	CameraController* currentController = nullptr;
	int ConstructedCellNum = 3;
	for( int i = ConstructedCellNum; i < m_controllers.size(); i++ ) {
		Vec2 pointX;
		switch( type )
		{
		case ORIGINAL_POLY:
			pointX = m_controllers[i]->GetOriginalVoronoiAnchorPointPos();
			break;
		case CURRENT_POLY:
			pointX = m_controllers[i]->GetVoronoiAnchorPointPos();
			break;
		case INITIALIZE_POLY:
			pointX = m_controllers[i]->GetOriginalVoronoiAnchorPointPos();
		default:
			break;
		}

		// construct hull X
		ConvexHull2 hullX = worldCameraHull;
		currentController = FindCurrentControllerContainsPointWithConstructedCellNum( pointX, ConstructedCellNum );
		std::vector<bool> controllerCheckStates( ConstructedCellNum, false );
		ConvexHull2 testHullA = m_controllers[0]->GetVoronoiHull();
		ConvexHull2 testHullB = m_controllers[1]->GetVoronoiHull();
		ConvexHull2 testHullC = m_controllers[2]->GetVoronoiHull();

		controllerCheckStates[currentController->GetIndex()] = true;
		while( currentController ) {
			Vec2 pointY;
			switch( type )
			{
			case ORIGINAL_POLY:
				pointY = currentController->GetOriginalVoronoiAnchorPointPos();
				break;
			case CURRENT_POLY:
				pointY = currentController->GetVoronoiAnchorPointPos();
				break;
			case INITIALIZE_POLY:
				pointY = currentController->GetOriginalVoronoiAnchorPointPos();
				break;
			default:
				break;
			}
			Vec2 center_XY = ( pointX + pointY ) / 2.f;
			Plane2 PB_XY = Plane2( ( pointY - pointX ).GetNormalized(), center_XY );
			Plane2 PB_YX = PB_XY.GetFlipped();
			hullX.AddPlane( PB_XY );
			currentController->AddPlaneToVoronoiHull( PB_YX, type );
			currentController = FindNextAdjacentControllerWithPlane( PB_YX, controllerCheckStates );
		}
		m_controllers[i]->SetVoronoiHullAndUpdateVoronoiPoly( hullX,type );
		ConstructedCellNum++;
	}
}

void CameraSystem::UpdateVoronoiPolyAndOffset()
{
//	 3 merges
	for( int i = 0; i < m_controllers.size(); i++ ) {
		Vec2 cameraPos = m_controllers[i]->GetCameraPos();
		Vec2 stencilOffset = cameraPos - m_noSplitCamera->GetPosition2D();
		m_controllers[i]->SetVoronoiStencilOffset( stencilOffset );

// 		Vec2 colorTargetOffset;
// 		if( IsControllerTotalSplit( i ) ) {
// 			colorTargetOffset = GetTotalSplitColorTargetOffset( i );
// 		}
// 		else if( IsControllerMerged( i ) ) {
// 			// two merged or three merged
// 			if( !m_controllers[i]->GetNeedRenderWhenMerged() ){ continue; }
// 			Polygon2 mergedPoly;
// 			 GetTotalMergedColorTargetOffsetAndMergedPoly( i, colorTargetOffset, mergedPoly );
// 			 std::vector<Vec2> testpoints;
// 			 mergedPoly.GetAllVerticesInWorld( testpoints );
// 			 m_controllers[i]->SetVoronoiPolygon( mergedPoly );
// 		}
// 		else {
// 			// blending
// 			// blending and merged
// 			std::vector<edgeAndThickness> blendingEdgeIndexesAndThickness;
// 			colorTargetOffset =	GetSplitToMergeBlendingColorTargetOffsetAndBlendingEdges( i, blendingEdgeIndexesAndThickness );
// 			for( int j = 0; j < blendingEdgeIndexesAndThickness.size(); j++ ) {
// 				m_controllers[i]->addBlendingEdgeIndexAndThickness( blendingEdgeIndexesAndThickness[j].first, blendingEdgeIndexesAndThickness[j].second );
// 			}
// 		}
// 
// 		m_controllers[i]->SetVoronoiColorTargetOffset( colorTargetOffset );
	}

	// update voronoi position state
	// update colorTargetOffset base on state
	std::vector<CameraController*> mergedControllers;
	std::vector<CameraController*> blendedControllers;
	UpdateVoronoiPosState( mergedControllers, blendedControllers );
	if( mergedControllers.size() == 0 && blendedControllers.size() == 0 ) { // all split
		// all split
		for( int i = 0; i < m_controllers.size(); i++ ) {
			m_controllers[i]->SetVoronoiColorTargetOffset( GetTotalSplitColorTargetOffset( i ) );
		}
	}
	else if( mergedControllers.size() == m_controllers.size() ) { // all merged
		UpdateAllMergedPolyAndOffset();
	}
	else if (blendedControllers.size() == m_controllers.size() ) { // all blended
		UpdateAllBlendedPolysAndOffsets();
	}
// 	switch (m_voronoiPosState)
// 	{
// 	case ALL_SPLIT: 
// 	{
// 		for( int i = 0; i < m_controllers.size(); i++ ) {
// 			m_controllers[i]->SetVoronoiColorTargetOffset( GetTotalSplitColorTargetOffset( i ));
// 		}
// 	}
// 		break;
// 	case ALL_MERGED: 
// 		UpdateTotalMergedPoly();
// 		break;
// 	case TWO_MERGED:
// 		UpdateAtLeastTwoMergedPoly();
// 		break;
// 	}
}

void CameraSystem::UpdateVoronoiPosState( std::vector<CameraController*>& mergedControllers, std::vector<CameraController*>& blendedControllers )
{
	for( int i = 0; i < m_controllers.size(); i++ ){
		if( IsControllerTotalSplit( i ) ) {
			continue;
		}
		else if( IsControllerMerged( i ) ) {
			mergedControllers.push_back( m_controllers[i] );
		}
		else {
			blendedControllers.push_back( m_controllers[i] );
		}
		
	}
}

void CameraSystem::ConstructVoronoiHullsWithThreePointsInCollinearOrder( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC )
{
	// construct planes for convex hull
	Vec2 center_AB = ( a + b ) / 2.f;
	Vec2 center_BC = ( b + c ) / 2.f;
	Plane2 PB_AB = Plane2( ( b - a ).GetNormalized(), center_AB );
	Plane2 PB_CB = Plane2( ( b - c ).GetNormalized(), center_BC );
	Plane2 PB_BA = PB_AB;
	Plane2 PB_BC = PB_CB;
	PB_BA.Flip();
	PB_BC.Flip();

	hullA.AddPlane( PB_AB );
	hullC.AddPlane( PB_CB );
	hullB.AddPlane( PB_BC );
	hullB.AddPlane( PB_BA );
}


void CameraSystem::ConstructVoronoiHullsWithThreePointsNotCollinear( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC )
{
	// construct convex hull for abc

	Vec2 center_AB = (a + b) / 2.f;
	Vec2 center_BC = (b + c) / 2.f;
	Vec2 center_AC = (a + c) / 2.f;
	Plane2 PB_AB = Plane2( (b - a).GetNormalized(), center_AB );
	Plane2 PB_AC = Plane2( (c - a).GetNormalized(), center_AC );
	Plane2 PB_BC = Plane2( (c - b).GetNormalized(), center_BC );

	Plane2 PB_BA = PB_AB.GetFlipped();
	Plane2 PB_CA = PB_AC.GetFlipped();
	Plane2 PB_CB = PB_BC.GetFlipped();

	hullA.AddPlane( PB_AB );
	hullA.AddPlane( PB_AC );
	hullB.AddPlane( PB_BA );
	hullB.AddPlane( PB_BC );
	hullC.AddPlane( PB_CA );
	hullC.AddPlane( PB_CB );
}

bool CameraSystem::DoesNeedBalanceVoronoiScreen()
{
	// Always check the original voronoi diagram
	if( m_controllers.size() <= 2 ){ return false; }

	AABB2 worldCamerBox = GetWorldCameraBox();
	switch( m_voronoiAreaCheckState )
	{
		case POLYGON_AREA: {
			float averageVoronoiArea = worldCamerBox.GetArea() / m_controllers.size();
			CameraController* minAreaController = FindControllerWithMinArea();
			return ( averageVoronoiArea - minAreaController->GetOriginalVoronoiArea() > m_expandVoronoiAreaThreshold );
		}
		case INCIRCLE_RADIUS: {
			float averageVoronoiIncircleRadius = GetAverageOriginalVoronoiInCircleRadius();
			float minVoronoiIncircleRadius = GetMinOriginalVoronoiInCircleRadius();
			return ( averageVoronoiIncircleRadius - minVoronoiIncircleRadius > m_voronoiInCircleRadiusThreshold );
		}
	}
	return false;
}

void CameraSystem::ExpandMinVoronoiPoly( )
{
	CameraController* minAreaController = FindControllerWithMinArea();
	int controllerIndex = minAreaController->GetIndex();
	Vec2 voronoiCenter = minAreaController->GetVoronoiPolygon().GetCenter();
	ConvexHull2& edges =  m_controllerVoronoiEdges[controllerIndex];
	for( Plane2& plane : edges.m_planes ) {

		for( int i = 0; i < m_controllerVoronoiEdges.size(); i++ ) {
			if( i == controllerIndex ){ continue; }

			ConvexHull2& possibleAcjacentEdges = m_controllerVoronoiEdges[i];
			for( Plane2& possiblePlane: possibleAcjacentEdges.m_planes ) {
				if( possiblePlane.IsFlippedWith( plane ) ) {
					possiblePlane.m_dist -= 1.f;
					break;
				}
			}
		}
		plane.m_dist += 1.f;
	} 
	AABB2 worldCameraBox = GetWorldCameraBox();
	for( int i = 0; i < m_controllerVoronoiEdges.size(); i++ ) {
		ConvexHull2 hull = m_controllerVoronoiEdges[i];
		hull.AddAABB2Planes( worldCameraBox );
		m_controllers[i]->SetVoronoiHullAndUpdateVoronoiPoly( hull, CURRENT_POLY );
	}
}

bool CameraSystem::ConstructAllAndIsAnyVoronoiVertex()
{
	m_voronoiVertices.clear();
	std::vector<Vec2> testPointsA;
	std::vector<Vec2> testPointsB;
	std::vector<Vec2> testPointsC;
	m_controllers[0]->GetVoronoiPolygon().GetPoints( testPointsA );
	m_controllers[1]->GetVoronoiPolygon().GetPoints( testPointsB );
	m_controllers[2]->GetVoronoiPolygon().GetPoints( testPointsC );
	for( int i = 0; i < m_controllers.size(); i++ ){
		for( int j = i + 1; j < m_controllers.size(); j++ ){
			for( int k = j + 1; k < m_controllers.size(); k++ ){
				Vec2 sharedPoint;
				if( GetSharedPointOfVoronoiPolygonABC( m_controllers[i]->GetVoronoiPolygon(), m_controllers[j]->GetVoronoiPolygon(), m_controllers[k]->GetVoronoiPolygon(), sharedPoint) ){
					m_voronoiVertices.push_back( VoronoiVertexAndControllersPair( sharedPoint, std::vector<CameraController*>{ m_controllers[i], m_controllers[j], m_controllers[k] }));
				}
				
			}
		}
	}
	std::vector<Vec2> testPointsD;
	std::vector<Vec2> testPointsE;
	std::vector<Vec2> testPointsF;
	m_controllers[0]->GetVoronoiPolygon().GetPoints( testPointsD );
	m_controllers[1]->GetVoronoiPolygon().GetPoints( testPointsE );
	m_controllers[2]->GetVoronoiPolygon().GetPoints( testPointsF );
	return !m_voronoiVertices.empty();
}

bool CameraSystem::RearrangeVoronoiVertexForMinVoronoiPoly( CameraController* minController, std::vector<Vec2> voronoiVertexPoss )
{
	// rearrange voronoi vertex 
	AABB2 worldCameraBox = GetWorldCameraBox();
	ConvexPoly2 voronoiPoly = minController->GetVoronoiPolygon();
	Vec2 polyCenter = voronoiPoly.GetCenter();
	for( Vec2 voronoiVertexPos : voronoiVertexPoss ) {
		Vec2 expandDirt = ( voronoiVertexPos - polyCenter ).GetNormalized();
		for( CameraController* controller : m_controllers ) {
			Vec2 expandedVoronoiVertexPos = voronoiVertexPos + expandDirt * m_expandVoronoiLeastStep;
			if( !worldCameraBox.IsPointInside( expandedVoronoiVertexPos ) ) {
				expandedVoronoiVertexPos = worldCameraBox.GetNearestPoint( expandedVoronoiVertexPos );
			}
			if( !controller->ReplaceVoronoiPointWithPoint( voronoiVertexPos, expandedVoronoiVertexPos ) ) {
				//No longer convex polygon
				return false;
			}
		}
	}
	return true;
}

void CameraSystem::ComputeAndUpdateOriginalVoronoiAnchorPoints( AABB2 worldCameraBox, AABB2 voronoiBox, bool isInitialized )
{
	for( CameraController* controller : m_controllers ) {
		Vec2 playerCameraPos = controller->GetCameraPos();
		Vec2 point	= voronoiBox.GetNearestPoint( playerCameraPos );
		controller->SetOriginalVoronoiAnchorPointPos( point );
		if( !isInitialized ) {
			controller->SetVoronoiAnchorPointPos( point );
		}
	}
	for( int i = 0; i < (int)m_controllers.size() - 1; i++ ) {
		for( int j = i + 1; j < m_controllers.size(); j++ ) {
			Vec2 controllerPosA = m_controllers[i]->GetOriginalVoronoiAnchorPointPos();
			Vec2 controllerPosB = m_controllers[j]->GetOriginalVoronoiAnchorPointPos();
			if( IsVec2MostlyEqual( controllerPosA, controllerPosB ) ) {
				controllerPosA -= Vec2( 0.5f );
				m_controllers[i]->SetOriginalVoronoiAnchorPointPos( controllerPosA );
			}
		}
	}
}

void CameraSystem::ComputeAndSetBalancedVoronoiTargetAnchorPoints( AABB2 worldCameraBox, AABB2 voronoiBox )
{
	// balance all points
	for( CameraController* controllerA : m_controllers ) {
		Vec2 anchorPointA = controllerA->GetVoronoiAnchorPointPos();
		float comparedValueA = 0.f;
		if( m_voronoiAreaCheckState == VoronoiAreaCheckState::POLYGON_AREA ) {
			comparedValueA = controllerA->GetVoronoiArea();
		}
		else if( m_voronoiAreaCheckState == VoronoiAreaCheckState::INCIRCLE_RADIUS ) {
			comparedValueA = controllerA->GetVoronoiInCircleRadius();
		}
		
		for( CameraController* controllerB : m_controllers ) {
			if( controllerA == controllerB ){ continue; }
			Vec2 anchorPointB = controllerB->GetVoronoiAnchorPointPos();

			float comparedValueB = 0.f ;
			float threshold = 0.f;
			if( m_voronoiAreaCheckState == VoronoiAreaCheckState::POLYGON_AREA ) {
				comparedValueB = controllerB->GetVoronoiArea();
				threshold = m_expandVoronoiAreaThreshold;	
			}
			else if( m_voronoiAreaCheckState == VoronoiAreaCheckState::INCIRCLE_RADIUS ) {
				comparedValueB = controllerB->GetVoronoiInCircleRadius();
				threshold = m_voronoiInCircleRadiusThreshold;
			}
			Vec2 moveDirt = Vec2::ZERO;
			if( comparedValueA < comparedValueB ) {
				moveDirt = ( anchorPointB - anchorPointA ).GetNormalized();
			}
			else {
				moveDirt = ( anchorPointA - anchorPointB ).GetNormalized();
			}
			float moveRatio = abs( comparedValueB - comparedValueA ) / threshold ; 
			moveRatio = ClampFloat( 0.f, 1.f, moveRatio );
			float moveDist	 = 0.f ;
			if( m_voronoiAreaCheckState == VoronoiAreaCheckState::POLYGON_AREA ) {
				moveDist = moveRatio * m_maxVoronoiAnchorPointMoveDistWithPolyArea;
			}
			else if( m_voronoiAreaCheckState == VoronoiAreaCheckState::INCIRCLE_RADIUS ) {
				moveDist = moveRatio * m_maxVoronoiAnchorPointMoveDistWithIncircleRadius;
			}
			controllerB->SetTargetVoronoiAnchorPointPos( controllerB->GetTargetVoronoiAnchorPointPos() + moveDirt * moveDist );
		}
	}
 	// balance only min area point
// 	CameraController* minController = nullptr;
// 	Vec2 minOriginalAnchorPoint = Vec2::ZERO;
// 	float minValue = 0;
// 
// 	// Deug
// 	Vec2 originalVoronoiPointA = m_controllers[0]->GetOriginalVoronoiAnchorPointPos();
// 	Vec2 originalVoronoiPointB = m_controllers[1]->GetOriginalVoronoiAnchorPointPos();
// 	Vec2 originalVoronoiPointC = m_controllers[2]->GetOriginalVoronoiAnchorPointPos();
// 	Vec2 originalVoronoiPointD = m_controllers[3]->GetOriginalVoronoiAnchorPointPos();
// 
// 	switch( m_voronoiAreaCheckState )
// 	{
// 		case POLYGON_AREA: {
// 			minController = FindControllerWithMinArea();
// 			minOriginalAnchorPoint = minController->GetOriginalVoronoiAnchorPointPos();
// 			minValue = minController->GetVoronoiArea();
// 		}
// 		break;
// 		case INCIRCLE_RADIUS: {
// 			minController = FindControllerWithMinRadius();
// 			minOriginalAnchorPoint = minController->GetOriginalVoronoiAnchorPointPos();
// 			minValue = minController->GetVoronoiInCircleRadius();
// 		}
// 		break;
// 	}
// 
// 	for( CameraController* controller : m_controllers ) {
// 		if( controller == minController ){ continue; }
//  		Vec2 originalAnchorPoint = controller->GetOriginalVoronoiAnchorPointPos();
// 		float comparedValue = 0.f;
// 		float maxMoveDist = 0.f;
// 		float difference = 0.f;
// 		switch ( m_voronoiAreaCheckState )
// 		{
// 			case POLYGON_AREA: {
// 				comparedValue = controller->GetOriginalVoronoiArea();
// 				maxMoveDist = m_maxVoronoiAnchorPointMoveDistWithPolyArea;
// 				difference = abs(comparedValue - minValue );
// 				if (( difference / minValue ) <  0.2f ){ continue; }
// 			}
// 			break;	
// 			case INCIRCLE_RADIUS: {
// 				comparedValue = controller->GetOriginalVoronoiInCircleRadius();
// 				maxMoveDist = m_maxVoronoiAnchorPointMoveDistWithIncircleRadius;
// 				difference = abs(comparedValue - minValue );
// 			}
// 		}
// 		Vec2 moveDirt = ( originalAnchorPoint - minOriginalAnchorPoint ).GetNormalized();
// 		
// // 		if( difference > m_expandVoronoiAreaThreshold ) {
// // 
// // 		}
//   		float moveDist = maxMoveDist;
// 		Vec2 targetPos = controller->GetTargetVoronoiAnchorPointPos(); 
// 		targetPos +=  moveDirt * moveDist;
// 		targetPos = worldCameraBox.GetNearestPointInside( targetPos );
// 
// 		//float blendTargetRatio = SmoothStep3( moveRatio );	// TOOD DEBUG
// 		//targetPos = RangeMapFromFloatToVec2( 0.f, 1.f, originalAnchorPoint, targetPos, blendTargetRatio );
// 		controller->SetTargetVoronoiAnchorPointPos( targetPos );
// 	}

// 	Vec2 targetVoronoiPointA = m_controllers[0]->GetTargetVoronoiAnchorPointPos();
// 	Vec2 targetVoronoiPointB = m_controllers[1]->GetTargetVoronoiAnchorPointPos();
// 	Vec2 targetVoronoiPointC = m_controllers[2]->GetTargetVoronoiAnchorPointPos();
// 	Vec2 targetVoronoiPointD = m_controllers[3]->GetTargetVoronoiAnchorPointPos();
}

void CameraSystem::InitializeVoronoiTargetPointWithVoronoiOriginalPoint()
{
	for( CameraController* controller : m_controllers ) {
		Vec2 originalAnchorPointPos = controller->GetOriginalVoronoiAnchorPointPos();
		controller->SetTargetVoronoiAnchorPointPos( originalAnchorPointPos );
	}
}

void CameraSystem::ReconstructVoronoiDiagramWithTargetAnchorPoints()
{
	AABB2 worldCameraBox	= GetWorldCameraBox();
	AABB2 voronoiBox		= GetVoronoiBox();
	for( CameraController* controller : m_controllers ) {
		Vec2 targetAnchorPoint		= controller->GetTargetVoronoiAnchorPointPos();
		Vec2 anchorPoint = controller->GetVoronoiAnchorPointPos();
		float distance = GetDistance2D( anchorPoint, targetAnchorPoint );
		float ratio = RangeMapFloat( 1.f, 10.f, 0.90f, 0.99f, distance );
		ratio = ClampFloat( 0.90f, 0.99f, ratio );
		anchorPoint = anchorPoint * ratio + targetAnchorPoint * ( 1 - ratio );
		controller->SetVoronoiAnchorPointPos( anchorPoint );
	}
	ConstructVoronoiDiagramForControllers( worldCameraBox, voronoiBox, CURRENT_POLY );
}

float CameraSystem::GetAverageVoronoiInCircleRadius()
{
	float totalRadius = 0.f;
	for( CameraController* controller : m_controllers ) {
		totalRadius += controller->GetVoronoiInCircleRadius();
	}
	return totalRadius / m_controllers.size(); 
}

float CameraSystem::GetAverageOriginalVoronoiInCircleRadius()
{
	float totalRadius = 0.f;
	for( CameraController* controller : m_controllers ) {
		totalRadius += controller->GetOriginalVoronoiInCircleRadius();
	}
	return totalRadius / m_controllers.size();
}

float CameraSystem::GetMinVoronoiInCircleRadius()
{
	float minRadius = m_controllers[0]->GetVoronoiInCircleRadius();
	for( CameraController* controller : m_controllers ) {
		float radius = controller->GetVoronoiInCircleRadius();
		if( radius < minRadius ) {
			minRadius = radius;
		}
	}
	return minRadius;
}

float CameraSystem::GetMinOriginalVoronoiInCircleRadius()
{
	float minRadius = m_controllers[0]->GetOriginalVoronoiInCircleRadius();
	for( CameraController* controller : m_controllers ) {
		float radius = controller->GetOriginalVoronoiInCircleRadius();
		if( radius < minRadius ) {
			minRadius = radius;
		}
	}
	return minRadius;
}

bool CameraSystem::IsControllerTotalSplit( int controllerIndex )
{
	for( int i = 0; i < 5; i++ ) {
		if( m_controllersSplitToMergeBlendCoeffs[controllerIndex][i] > 0.f ) {
			return false;
		}
	}
	return true;
}

bool CameraSystem::IsControllerMerged( int controllerIndex )
{
	for( int i = 0; i < 5; i++ ) {
		if( m_controllersSplitToMergeBlendCoeffs[controllerIndex][i] == 1.f ){
			return true;
		}
	}
	return false;
}

bool CameraSystem::IsControllerNeedRenderWhenMerged( int controllerIndex )
{
	// render when minimun index
	for( int i = 0; i < MAX_CAMERA_CONTROLLER_NUM; i++ ) {
		if( m_controllersSplitToMergeBlendCoeffs[controllerIndex][i] == 1.f ) {
			if( i < controllerIndex ){ return false; }
		}
	}
	return true;
}

bool CameraSystem::GetSharedPointOfVoronoiPolygonABC( ConvexPoly2 polygonA, ConvexPoly2 polygonB, ConvexPoly2 polygonC, Vec2& sharedPoint )
{
	std::vector<Vec2> polyAWorldPoints;
	std::vector<Vec2> polyBWorldPoints;
	std::vector<Vec2> polyCWorldPoints;
	std::vector<Vec2> sharedPoints;
	polygonA.GetPoints( polyAWorldPoints );
	polygonB.GetPoints( polyBWorldPoints );
	polygonC.GetPoints( polyCWorldPoints );
	for( int i = 0; i < polyAWorldPoints.size(); i++ ) {
		for( int j = 0; j < polyBWorldPoints.size(); j++ ) {
			if( IsVec2MostlyEqual( polyAWorldPoints[i], polyBWorldPoints[j] ) ) {
				sharedPoints.push_back( polyAWorldPoints[i] );
			}
		}
	}
	for( int i = 0; i < sharedPoints.size(); i++ ) {
		for( int j = 0; j < polyCWorldPoints.size(); j++ ) {
			if( IsVec2MostlyEqual( sharedPoints[i], polyCWorldPoints[j] ) ) {
				sharedPoint = sharedPoints[i];
				return true;
			}
		}
	}

	return false;
}

bool CameraSystem::GetAllVoronoiVertexPosWithController( const CameraController* controller, std::vector<Vec2>& vertexPosArray )
{
	bool result = false;
	for( VoronoiVertexAndControllersPair vertex : m_voronoiVertices ) {
		for( CameraController* verifiedController : vertex.second ) {
			if( verifiedController == controller ) {
				vertexPosArray.push_back(vertex.first);
				if( !result ){
					result = true;
				}
			}
		}
	}
	return result;
}

CameraController* CameraSystem::FindCurrentControllerContainsPointWithConstructedCellNum( Vec2 point, int constructedCellNum )
{
	for( int i = 0; i < constructedCellNum; i++ ) {
		ConvexHull2 hull = m_controllers[i]->GetVoronoiHull();
		if( hull.IsPointInside( point ) ) {
			return m_controllers[i];
		}
	}
	ERROR_RECOVERABLE( "Should find one contains point" );
	return nullptr;
}

CameraController* CameraSystem::FindControllerWithMinArea()
{
	CameraController* result = nullptr;
	float minArea = m_controllers[0]->GetVoronoiArea();
	result = m_controllers[0];
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->GetVoronoiArea() < minArea ) {
			result = m_controllers[i];
			minArea = result->GetVoronoiArea();
		}
	}
	if( IsFloatMostlyEqual( minArea, 0 ) || result == nullptr ) {
		ERROR_RECOVERABLE(" min area should not be zero!" );
	}
	return result;
}

CameraController* CameraSystem::FindControllerWithMinRadius()
{
	CameraController* result = nullptr;
	float minRadius = m_controllers[0]->GetVoronoiInCircleRadius();
	result = m_controllers[0];
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[i]->GetVoronoiInCircleRadius() < minRadius ) {
			result = m_controllers[i];
			minRadius = result->GetVoronoiInCircleRadius();
		}
	}
	if( IsFloatMostlyEqual( minRadius, 0 ) || result == nullptr ) {
		ERROR_RECOVERABLE( " min Radius should not be zero!" );
	}
	return result;
}

CameraController* CameraSystem::FindNextAdjacentControllerWithPlane( Plane2 plane, std::vector<bool>& controllerCheckStates )
{
	for( int i = 0; i < controllerCheckStates.size(); i++ ) {
		CameraController* tempController = m_controllers[i];
		ConvexHull2 hull = tempController->GetVoronoiHull();
		if( hull.IsSlicedByPlane( plane ) && !controllerCheckStates[i] ) {
			controllerCheckStates[i] = true;
			return tempController;
		}
	}
	return nullptr;
}

int CameraSystem::FindRenderControllerIndex()
{
	return 0;
}

void CameraSystem::GetMergedPolygonWithTwoPolygonShareOneEdge( const ConvexPoly2& polyA, const ConvexPoly2& polyB, ConvexPoly2& mergedPoly )
{
	// find same point 
	// reorder the point index
	// extand points to new polygon
	std::vector<Vec2> mergedPolyWorldPoints;
	std::vector<Vec2> polyAWorldPoints;
	std::vector<Vec2> polyBWorldPoints;
	polyA.GetPoints( polyAWorldPoints );
	polyB.GetPoints( polyBWorldPoints );
	mergedPolyWorldPoints.reserve( polyAWorldPoints.size() + polyBWorldPoints.size() );
	int samePointIndexInPolyA = 0;
	int samePointIndexInPolyB = 0;
	bool isSamePointFound = false;
	for( int i = 0; i < polyAWorldPoints.size(); i++ ) {
		for( int j = 0; j < polyBWorldPoints.size(); j++ ) {
			if( IsVec2MostlyEqual( polyAWorldPoints[i], polyBWorldPoints[j] ) ) {
				samePointIndexInPolyA = i;
				samePointIndexInPolyB = j;
				isSamePointFound = true;
				break;
			}
		}
		if( isSamePointFound ) { break; }
	}

	// rearrange same point as start of points
	// two same point should be first and back of one vector and first and second of another vector
	while( samePointIndexInPolyA != 0 )
	{
		Vec2 tempPoint = polyAWorldPoints.front();
		polyAWorldPoints.erase( polyAWorldPoints.begin() );
		polyAWorldPoints.push_back( tempPoint );
		samePointIndexInPolyA--;
	}

	while( samePointIndexInPolyB != 0 ) {
		Vec2 tempPoint = polyBWorldPoints.front();
		polyBWorldPoints.erase( polyBWorldPoints.begin() );
		polyBWorldPoints.push_back( tempPoint );
		samePointIndexInPolyB--;
	}

	if( IsVec2MostlyEqual( polyAWorldPoints.back(), polyBWorldPoints[1] ) ) {
		for( int i = 0; i < polyAWorldPoints.size(); i++ ) {
			mergedPolyWorldPoints.push_back( polyAWorldPoints[i] );
		}

		for( int i = 2; i < polyBWorldPoints.size(); i++ ) {
			mergedPolyWorldPoints.push_back( polyBWorldPoints[i] );
		}
	}
	else {
		for( int i = 0; i < polyBWorldPoints.size(); i++ ) {
			mergedPolyWorldPoints.push_back( polyBWorldPoints[i] );
		}

		for( int i = 2; i < polyAWorldPoints.size(); i++ ) {
			mergedPolyWorldPoints.push_back( polyAWorldPoints[i] );
		}
	}

	mergedPoly = ConvexPoly2( mergedPolyWorldPoints );
}


Vec2 CameraSystem::GetTotalSplitColorTargetOffset( int controllerIndex )
{
	ConvexPoly2 splitPolygon = m_controllers[controllerIndex]->GetVoronoiPolygon();
	Vec2 splitPolygonCenter = splitPolygon.GetCenter();
	Vec2 splitColorTargetOffset = splitPolygonCenter - m_noSplitCamera->GetPosition2D();
	return splitColorTargetOffset;
}

Vec2 CameraSystem::GetSplitToMergeBlendingColorTargetOffsetAndBlendingEdges( int controllerIndex, std::vector<edgeAndThickness>& blendingEdgeIndexes )
{
	// two controller blending
	// three controller blending
	// two controller merged one blending
	std::vector<int> blendingControllerIndexes;
	std::vector<int> mergedControllerIndexes;
	Vec2 result;
	Vec2 splitColorTargetOffset = GetTotalSplitColorTargetOffset( controllerIndex );
	Vec2 mergedTargetOffset;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllersSplitToMergeBlendCoeffs[controllerIndex][i] == 0.f ) { 
			continue;
		}
		else if( m_controllersSplitToMergeBlendCoeffs[controllerIndex][i] == 1.f ) {
			mergedControllerIndexes.push_back( i );
		}	
		else {
			blendingControllerIndexes.push_back( i );
		}
	}
	if( mergedControllerIndexes.size() == 0 ) {
		// two blending or three blending
 		Vec2 cameraPos = m_controllers[controllerIndex]->GetCameraPos();
		Vec2 blendingColorTargetOffset = Vec2::ZERO;
 		for( int i = 0; i < blendingControllerIndexes.size(); i++ ) {
			blendingColorTargetOffset += GetBlendingColorTargetOffsetAndBlendingEdgeIndexesOfTwoControllers( controllerIndex, blendingControllerIndexes[i], blendingEdgeIndexes );
 		}
		blendingColorTargetOffset /= (float)blendingControllerIndexes.size();
		return blendingColorTargetOffset;
	}
	else if( mergedControllerIndexes.size() == 1 ) {
		// two controller merged one blending
		Vec2 cameraPos = m_controllers[controllerIndex]->GetCameraPos();
		Vec2 blendingColorTargetOffset = Vec2::ZERO;
		for( int i = 0; i < mergedControllerIndexes.size(); i++ ) {
			blendingColorTargetOffset += GetBlendingColorTargetOffsetAndBlendingEdgeIndexesOfTwoControllers( controllerIndex, mergedControllerIndexes[i], blendingEdgeIndexes );
		}
		blendingColorTargetOffset /= (float)mergedControllerIndexes.size();
		return blendingColorTargetOffset;

	}
	else {
		ERROR_RECOVERABLE(" should not have this situation!" );
		return Vec2::ZERO;

	}
}


Vec2 CameraSystem::GetBlendingColorTargetOffsetAndBlendingEdgeIndexesOfTwoControllers( int controllerIndexA, int controllerIndexB, std::vector<edgeAndThickness>& edges )
{
	ConvexPoly2 mergedPoly;
	LineSegment2 blendEdge = GetBlendingEdgeAndMergedVoronoiPolygonWithTwoControllers( m_controllers[controllerIndexA], m_controllers[controllerIndexB], mergedPoly );
	edges.push_back( edgeAndThickness( blendEdge, m_controllersSplitToMergeBlendCoeffs[controllerIndexA][controllerIndexB] ) );
	
	Vec2 mergedPolyCenter = mergedPoly.GetCenter();
	Vec2 cameraPos = m_controllers[controllerIndexA]->GetCameraPos();
	Vec2 anotherCameraPos = m_controllers[controllerIndexB]->GetCameraPos();
	Vec2 blendingMergedColorTargetOffset = (cameraPos + anotherCameraPos) / 2.f - m_noSplitCamera->GetPosition2D();
	Vec2 displacement = ( cameraPos - anotherCameraPos ) / 2.f;
	Vec2 mergedColorTargetOffset = mergedPolyCenter - m_noSplitCamera->GetPosition2D() + displacement ;
	Vec2 splitColorTargetOffset = GetTotalSplitColorTargetOffset( controllerIndexA );
	float splitBlendCoeff = m_controllersSplitToMergeBlendCoeffs[controllerIndexA][controllerIndexB];

	Vec2 result = RangeMapFromFloatToVec2( 0.f, 1.f, splitColorTargetOffset, blendingMergedColorTargetOffset, splitBlendCoeff );

	if( splitBlendCoeff > 0.f ) {
		splitBlendCoeff = SmoothStep3( splitBlendCoeff );
		result = ClampRangeMapFromFloatToVec2( 0.f, 1.f, result, mergedColorTargetOffset, splitBlendCoeff );
	}
	return result;
}

Vec2 CameraSystem::GetMergedColorTargetOffset( std::vector<int> controllerIndexes )
{
	Vec2 result = Vec2::ZERO;
	for( int i = 0; i < controllerIndexes.size(); i++ ) {
		Vec2 cameraPos = m_controllers[controllerIndexes[i]]->GetCameraPos();
		result += cameraPos;
	}
	result /= (float)controllerIndexes.size();
	return result;
}

void CameraSystem::GetTotalMergedColorTargetOffsetAndMergedPoly( int controllerIndexA, Vec2& mergedColorTargetOffset, ConvexPoly2& mergedPoly )
{
	std::vector<int> mergedControllerIndexes;
	std::vector<int> blendingControllerIndexes;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( i == controllerIndexA ){ continue; }
		if( m_controllersSplitToMergeBlendCoeffs[controllerIndexA][i] == 1.f ) {
			mergedControllerIndexes.push_back( i );
		}
		else if( m_controllersSplitToMergeBlendCoeffs[controllerIndexA][i] > 0.f ) {
			blendingControllerIndexes.push_back( i );
		}
	}
	if( mergedControllerIndexes.size() == 1 ) {
		for( int i = 0; i < 5; i++ ) {
			if( m_controllersSplitToMergeBlendCoeffs[mergedControllerIndexes[0]][i] == 1.f && i != controllerIndexA ) {
				mergedControllerIndexes.push_back( i );
			}
		}
	}

	if( mergedControllerIndexes.size() == 1 ) {
		// need to blending
		GetMergedVoronoiPolygonWithTwoControllers( m_controllers[controllerIndexA], m_controllers[mergedControllerIndexes[0]], mergedPoly );
		
		Vec2 mergedPolygonCenter = mergedPoly.GetCenter();
		Vec2 anotherCameraPos		= m_controllers[mergedControllerIndexes[0]]->GetCameraPos();
		Vec2 cameraPos				= m_controllers[controllerIndexA]->GetCameraPos();
		Vec2 displacement			= (cameraPos - anotherCameraPos) / 2.f;
		//mergedColorTargetOffset = mergedCameraPos - m_noSplitCamera->GetPosition2D(); 
		mergedColorTargetOffset = mergedPolygonCenter - m_noSplitCamera->GetPosition2D() + displacement ;
		if( blendingControllerIndexes.size() == 0 ) {
			return;
		}
		else if( blendingControllerIndexes.size() == 1 ) {
			float blendingCoeff = m_controllersSplitToMergeBlendCoeffs[controllerIndexA][blendingControllerIndexes[0]];
			Vec2 blendingCameraPos = m_controllers[blendingControllerIndexes[0]]->GetCameraPos();
			Vec2 threeMergedTargetOffset = ( anotherCameraPos + blendingCameraPos + cameraPos ) / 3.f - m_noSplitCamera->GetPosition2D();
			mergedColorTargetOffset = RangeMapFromFloatToVec2( 0.f, 1.f, mergedColorTargetOffset, threeMergedTargetOffset, blendingCoeff );
		}
		else {
			ERROR_RECOVERABLE( "should not have other situation when merging!" );
		}
	}
	else if( mergedControllerIndexes.size() == 2 ) {
		Vec2 cameraPos 	= m_controllers[controllerIndexA]->GetCameraPos();
		Vec2 totalCameraPos = cameraPos;
		for( int i = 0; i < mergedControllerIndexes.size(); i++ ) {
			totalCameraPos += m_controllers[mergedControllerIndexes[i]]->GetCameraPos();
		}
		totalCameraPos /= 3.f;
		GetMergedVoronoiPolygonWithThreeControllers( m_controllers[controllerIndexA], m_controllers[mergedControllerIndexes[0]],m_controllers[mergedControllerIndexes[1]], mergedPoly );
		Vec2 mergedPolyCenter = mergedPoly.GetCenter();
		Vec2 displacement = cameraPos - totalCameraPos;
		mergedColorTargetOffset = mergedPolyCenter - m_noSplitCamera->GetPosition2D() + displacement;
	}
}

void CameraSystem::UpdateAllMergedPolyAndOffset()
{
	AABB2 worldCameraBox = GetWorldCameraBox();
	Vec2 centerPos = m_controllers[0]->GetCameraPos();
	Vec2 playerCenterPos = m_controllers[0]->m_player->GetPosition();
	for( int i = 1; i < m_controllers.size(); i++ ) {
		centerPos += m_controllers[i]->GetCameraPos();
		playerCenterPos += m_controllers[i]->m_player->GetPosition();
		m_controllers[i]->SetNeedRenderWhenMerged( false );
	}
	playerCenterPos /= m_controllers.size();
	centerPos /= m_controllers.size();
	worldCameraBox.SetCenter( centerPos );
	ConvexPoly2 poly = ConvexPoly2::MakeConvexPolyFromAABB2( GetWorldCameraBox() );
	m_controllers[0]->SetVoronoiPolygon( poly );
	m_controllers[0]->SetVoronoiColorTargetOffset( Vec2::ZERO );
}

void CameraSystem::UpdateAllBlendedPolysAndOffsets()
{
	
}

void CameraSystem::UpdateAtLeastTwoMergedPoly()
{
	// find two merged controller
	// merged as one
	// handle last one
	CameraController* mergedController = nullptr;
	for( int i = 0; i < m_controllers.size(); i++ ) {
		if( m_controllers[] )
	}
}

void CameraSystem::GetMergedVoronoiPolygonWithTwoControllers( CameraController* a, CameraController* b, ConvexPoly2& mergedPoly )
{
	ConvexPoly2 polyA = a->GetVoronoiPolygon();
	ConvexPoly2 polyB = b->GetVoronoiPolygon();
	GetMergedPolygonWithTwoPolygonShareOneEdge( polyA,  polyB, mergedPoly );
}

LineSegment2 CameraSystem::GetBlendingEdgeAndMergedVoronoiPolygonWithTwoControllers( CameraController* a, CameraController* b, ConvexPoly2& mergedPoly )
{
	ConvexPoly2 polyA = a->GetVoronoiPolygon();
	ConvexPoly2 polyB = b->GetVoronoiPolygon();
	GetMergedPolygonWithTwoPolygonShareOneEdge( polyA, polyB, mergedPoly );
	std::vector<LineSegment2> edgesA;
	std::vector<LineSegment2> edgesB;
	polyA.GetEdges( edgesA );
	polyB.GetEdges( edgesB );
	for( int i = 0; i < edgesA.size(); i++ ) {
		for( int j = 0; j < edgesB.size(); j++ ) {
			if( IsLineSeg2MostlyEqual( edgesA[i], edgesB[j] ) ) {
				return edgesA[i];
			}
		}
	}
	return LineSegment2();
}

void CameraSystem::GetMergedVoronoiPolygonWithThreeControllers( CameraController* a, CameraController* b, CameraController* c, ConvexPoly2& mergedPoly )
{
	ConvexPoly2 polyA = a->GetVoronoiPolygon();
	ConvexPoly2 polyB = b->GetVoronoiPolygon();
	ConvexPoly2 polyC = c->GetVoronoiPolygon();
	std::vector<Vec2> polyAWorldPoints;
	std::vector<Vec2> polyBWorldPoints;
	std::vector<Vec2> polyCWorldPoints;
	std::vector<Vec2> mergedWorldPoints;
	polyAWorldPoints.reserve( polyA.GetPointsCount() );
	polyBWorldPoints.reserve( polyB.GetPointsCount() );
	polyCWorldPoints.reserve( polyC.GetPointsCount() );
	mergedWorldPoints.reserve( polyAWorldPoints.size() + polyBWorldPoints.size() + polyCWorldPoints.size() );
	polyA.GetPoints( polyAWorldPoints );
	polyB.GetPoints( polyBWorldPoints );
	polyC.GetPoints( polyCWorldPoints );

	for( Vec2 pointA : polyAWorldPoints ) {
		bool isPointExist = false;
		for( Vec2 pointB : mergedWorldPoints ) {
			if( IsVec2MostlyEqual( pointA, pointB ) ) {
				isPointExist = true;
				break;
			}
		}
		if( !isPointExist ) {
			mergedWorldPoints.push_back( pointA );
		}
	}
	for( Vec2 pointA : polyBWorldPoints ) {
		bool isPointExist = false;
		for( Vec2 pointB : mergedWorldPoints ) {
			if( IsVec2MostlyEqual( pointA, pointB ) ) {
				isPointExist = true;
				break;
			}
		}
		if( !isPointExist ) {
			mergedWorldPoints.push_back( pointA );
		}
	}
	for( Vec2 pointA : polyCWorldPoints ) {
		bool isPointExist = false;
		for( Vec2 pointB : mergedWorldPoints ) {
			if( IsVec2MostlyEqual( pointA, pointB ) ) {
				isPointExist = true;
				break;
			}
		}
		if( !isPointExist ) {
			mergedWorldPoints.push_back( pointA );
		}
	}
	mergedPoly = ConvexPoly2::MakeConvexPolyFromPointCloud( mergedWorldPoints );
}


