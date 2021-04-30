#include "CameraController.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "../../Thesis/Code/Game/Camera/CameraSystem.hpp"

#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/ConvexPoly2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/MeshUtils.hpp"

#include <chrono>

extern RenderContext*	g_theRenderer;
extern Camera*			g_gameCamera;
extern Game*			g_theGame;


Texture* CameraController::s_stencilTexture = nullptr;


CameraController::CameraController( CameraSystem* owner, Player* player, Camera* camera )
	:m_owner( owner )
	,m_player( player )
	,m_camera( camera )
{
	m_timer = new Timer();
	m_splitCamera = Camera::CreateOrthographicCamera( g_theRenderer, Vec2( GAME_CAMERA_MIN_X, GAME_CAMERA_MIN_Y ), Vec2( GAME_CAMERA_MAX_X, GAME_CAMERA_MAX_Y ) );
	m_splitCamera->EnableClearColor( Rgba8::BLACK );
	m_camera->EnableClearColor( Rgba8::GRAY );
	m_offsetBuffer			= new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_voronoiOffsetBuffer	= new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );

	m_boxStencilShader		= g_theRenderer->GetOrCreateShader( "data/Shader/boxStencil.hlsl" );
	m_voronoiStencilShader	= g_theRenderer->GetOrCreateShader( "data/Shader/VoronoiStencil.hlsl" );
	m_voronoiDiffuseShader	= g_theRenderer->GetOrCreateShader( "data/Shader/VoronoiDiffuse.hlsl" );

	
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	int bufferSizeNum = backBuffer->GetHeight() * backBuffer->GetWidth();
	std::vector<Rgba8> stencilData( bufferSizeNum, Rgba8::BLACK );
	if( s_stencilTexture == nullptr ) {
		s_stencilTexture = g_theRenderer->CreateRenderTargetWithSizeAndData( backBuffer->GetSize(), stencilData.data() );
	}
}

void CameraController::BeginFrame()
{
	m_blendingEdgeAndThickness.clear();
	m_stencilVertices.clear();
}

CameraController::~CameraController()
{
	// leave map delete player
	// leave camera to future
	//delete m_timer;
	SELF_SAFE_RELEASE(m_splitCamera);
	SELF_SAFE_RELEASE(m_offsetBuffer);
	SELF_SAFE_RELEASE(m_voronoiOffsetBuffer);
	SELF_SAFE_RELEASE(m_timer);
}

void CameraController::Update( float deltaSeconds )
{
	m_playerPos				= m_player->GetPosition();
	m_cameraPos				= m_camera->GetPosition();
	m_cameraWindowCenterPos	= m_cameraWindow.GetCenter();

	UpdateCameraWindow( deltaSeconds );
	UpdateCameraFrame( );
	UpdateCameraShake( deltaSeconds );
	UpdateMultipleCameraSettings( deltaSeconds );
	SmoothMotion();
	BoundCameraPosInsideWindow();
}

void CameraController::Render()
{
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	m_colorTarget = g_theRenderer->AcquireRenderTargetMatching( backBuffer );

	// draw render area to stencil texture
	UpdateMultipleCameraOffsetAndBuffer();

	RenderToStencilTexture();
	RenderToTargetTexture();
}

void CameraController::EndFrame()
{	
}

void CameraController::DebugRender()
{
	if( m_isDebug ) {
		if( m_owner->GetDoesDebugCameraWindow() ) {
			g_theRenderer->SetDiffuseTexture( nullptr );
			Vec2 windowPoints[4];
			m_cameraWindow.GetCornerPositions( windowPoints );
			for( int i = 0; i < 3; i++ ) {
				g_theRenderer->DrawLine( windowPoints[i], windowPoints[i + 1], 0.1f, Rgba8::BLUE );
			}
			g_theRenderer->DrawLine( windowPoints[0], windowPoints[3], 0.1f, Rgba8::BLUE );
		}
		if( m_owner->GetDoesDebugCameraPos() ) {
			g_theRenderer->SetDiffuseTexture( nullptr );
			g_theRenderer->DrawCircle( m_goalCameraPos, 0.1f, 0.1f, Rgba8::BLACK );
			g_theRenderer->DrawCircle( m_smoothedGoalCameraPos, 0.1f, 0.1f, Rgba8::RED );
		}
	}
}

void CameraController::DebugCameraInfoAt( Vec4 pos )
{
	Vec2 playerPos		= m_player->GetPosition();
	Vec2 cameraPos		= m_camera->GetPosition();
	Vec2 cameraGoalPos	= m_goalCameraPos;
	Strings debugStrings;
	float currentFactorSeconds = m_contributionRatioStableSeconds - (float)m_timer->GetSecondsRemaining();

	switch( m_owner->GetDebugMode() )
	{
		case CONTROLLER_INFO: {
			std::string playerPosText					= std::string( "Player pos =  " + playerPos.ToString() );
			std::string cameraPosText					= std::string( "Camera pos = " + cameraPos.ToString() );
			std::string cameraGoalPosText				= std::string( "Goal Camera pos = " + cameraGoalPos.ToString() );
			std::string asymptoticValueText				= std::string( "Asymptotic Value = " + std::to_string( m_asymptoticValue ) );
			std::string traumaText						= std::string( "Trauma = " + std::to_string( m_trauma ) );
			std::string currentMultipleStableFactorText	= std::string( "multiple factor = " + std::to_string( m_currentMultipleCameraContributionsRatio ));
			std::string goalMultipleStableFactorText	= std::string( "goal multiple factor = " + std::to_string( m_goalMultipleFactor ));
			std::string currentFactorSecondsText		= Stringf( "current factor seconds: %.2f", currentFactorSeconds );
			std::string totalFactorSecondsText			= Stringf( "total factor seconds: %.2f", m_contributionRatioStableSeconds );

			debugStrings.push_back( playerPosText );
			debugStrings.push_back( cameraPosText );
			debugStrings.push_back( cameraGoalPosText );
			debugStrings.push_back( asymptoticValueText );
			debugStrings.push_back( traumaText );
			debugStrings.push_back( currentMultipleStableFactorText );
			debugStrings.push_back( goalMultipleStableFactorText );
			debugStrings.push_back( currentFactorSecondsText );
			debugStrings.push_back( totalFactorSecondsText );
		}
		break;
		case VORONOI_INFO: {
			std::string voronoiAreaText						= Stringf( "voronoi area is: %.2f" , m_voronoiPolyArea );
			std::string originalVoronoiAreaText				= Stringf( "original voronoi area is: %.2f" , m_originalVoronoiPolyArea );
			std::string voronoiInCircleRadiusText			= Stringf( "voronoi radius is: %.2f" , m_voronoiInCircleRadius );
			std::string originalVoronoiInCircleRadiusText	= Stringf( "original voronoi radius is: %.2f" , m_originalvoronoiInCircleRadius );
			std::string originalAnchorPointText				= std::string( "original Anchor is: ") + m_originalVoronoiAnchorPointPos.ToString() ;
			std::string currentAnchorPointText				= Stringf( "current Anchor is: ") + m_voronoiAnchorPointPos.ToString() ;
			std::string targetAnchorPointText				= Stringf( "target Anchor is: ") + m_targetVoronoiAnchorPointPos.ToString() ;


			debugStrings.push_back( originalVoronoiInCircleRadiusText );
			debugStrings.push_back( voronoiInCircleRadiusText );
			debugStrings.push_back( voronoiAreaText );
			debugStrings.push_back( originalVoronoiAreaText );
			debugStrings.push_back( originalAnchorPointText );
			debugStrings.push_back( currentAnchorPointText );
			debugStrings.push_back( targetAnchorPointText );
		}
		break;

	}

	DebugAddScreenStrings( pos, Vec2::ZERO, 1.5f, Rgba8( 255, 255, 255, 125), debugStrings );
}

Vec2 CameraController::GetCuePos() const
{
	Map* currentMap = g_theGame->GetCurrentMap();
	return currentMap->GetCuePos( m_index );
}

Polygon2 const& CameraController::GetVoronoiPolygon() const 
{
	return m_voronoiPolygon;
}

Polygon2 const& CameraController::GetOriginalVoronoiPolygon() const
{
	return m_originalVoronoiPolygon;
}

void CameraController::SetIndex( int index )
{
	m_index = index;
}

void CameraController::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
}

void CameraController::SetIsSmooth( bool isSmooth )
{
	m_isSmooth = isSmooth;
}

void CameraController::SetAllowMerge( bool allowMerge )
{
	m_allowMerge = allowMerge;
}

void CameraController::SetAllowBlend( bool allowBlend )
{
	m_allowBlend = allowBlend;
}

void CameraController::SetAsymptoticValue( float value )
{
	m_asymptoticValue = value;
}

void CameraController::SetMaxAsymptoticValue( float maxValue )
{
	m_maxAsymptotic = maxValue;
}

void CameraController::SetMinAsymptoticValue( float minValue )
{
	m_minAsymptotic = minValue;
}

void CameraController::SetMaxDeltaDistance( float maxDeltaDist )
{
	m_maxDeltaDist = maxDeltaDist;
}

void CameraController::SetTrauma( float trauma )
{
	m_trauma = trauma;
}

void CameraController::AddTrauma( float addTrauma )
{
	m_trauma += addTrauma;
}

void CameraController::SetForwardVelocityFocusDist( float dist )
{
	m_fwdVelFocusDist = dist;
}

void CameraController::SetAimFocusDist( float dist )
{
	m_aimFocusDist = dist;
}

void CameraController::SetForwardVelocityFocusRatio( float ratio )
{
	m_fwdVelFocusRatio = ratio;
}

void CameraController::SetAimFocusRatio( float ratio )
{
	m_aimFocusRatio = ratio;
}

void CameraController::SetCueFocusRatio( float ratio )
{
	m_cueFocusRatio = ratio;
}

void CameraController::SetPositionalShakeMaxDist( float maxDist )
{
	m_maxShakePosDist = maxDist;
}

void CameraController::SetRotationalShakeMaxDeg( float maxDeg )
{
	m_maxShakeRotDeg = maxDeg;
}

void CameraController::SetMaxEdgeThickness( float maxThickness )
{
	m_maxedgeThickness = maxThickness;
}

void CameraController::SetSplitScreenEdgeColor( Rgba8 color )
{
	m_splitScreenEdgeColor = color;
}

void CameraController::SetMultipleCameraStableFactorNotStableUntil( float totalSeconds, float goalFactor )
{
	m_ismultipleFactorStable = false;
	m_timer->SetSeconds( (double)totalSeconds );
	g_theConsole->DebugLogf( "set, player: %d, start: %.2f, duration: %.2f", m_player->GetPlayerIndex(), m_timer->m_startSeconds, m_timer->m_durationSeconds );
	m_startMultipleFactor = m_currentMultipleCameraContributionsRatio;
	m_goalMultipleFactor = goalFactor;
	m_contributionRatioStableSeconds = totalSeconds;
}

void CameraController::SetVoronoiPolygon( Polygon2 const& poly )
{
	m_voronoiPolygon = poly;
	m_voronoiPolyArea = m_voronoiPolygon.GetArea();
}

void CameraController::SetOriginalVoronoiPolygon( Polygon2 const& poly )
{
	m_originalVoronoiPolygon = poly;
	m_originalVoronoiPolyArea = m_originalVoronoiPolygon.GetArea();
}

void CameraController::SetVoronoiHullAndUpdateVoronoiPoly( ConvexHull2 const& hull, PolyType type )
{
	m_voronoiHull = hull;
	UpdateVoronoiPoly( type );
}

void CameraController::AddPlaneToVoronoiHull( Plane2 const& plane, PolyType type )
{
	m_voronoiHull.AddPlane( plane );
	UpdateVoronoiPoly( type );
}

void CameraController::SetVoronoiStencilOffset( Vec2 offset )
{
	m_voronoiStencilOffset = offset;
}

void CameraController::SetVoronoiColorTargetOffset( Vec2 offset )
{
	m_voronoiColorTargetOffset = offset;
}

void CameraController::SetMultipleCameraRenderOffset( Vec2 offset )
{
	m_multipleCameraRenderOffset = offset;
}

void CameraController::SetVoronoiAnchorPointPos( Vec2 voronoiAnchorPoint )
{
	m_voronoiAnchorPointPos = voronoiAnchorPoint;
}

void CameraController::SetTargetVoronoiAnchorPointPos( Vec2 targetVoronoiAnchorPoint )
{
	m_targetVoronoiAnchorPointPos = targetVoronoiAnchorPoint;
}

void CameraController::SetOriginalVoronoiAnchorPointPos( Vec2 originalVoronoiAnchorPoint )
{
	m_originalVoronoiAnchorPointPos = originalVoronoiAnchorPoint;
}

void CameraController::SetNeedRenderWhenMerged( bool doesNeedRenderWhenMerged )
{
	m_needRenderWhenMerged = doesNeedRenderWhenMerged;
}

void CameraController::addBlendingEdgeIndexAndThickness( LineSegment2 const& line, float thicknessRatio )
{

	m_blendingEdgeAndThickness.push_back( edgeAndThickness( line, thicknessRatio ) );
}

bool CameraController::ReplaceVoronoiPointWithPoint( Vec2 replacedPoint, Vec2 newPoint )
{
	bool result = m_voronoiPolygon.ReplacePointWithPoint( replacedPoint, newPoint );
	UpdateVoronoiHull();
	return result;
}

void CameraController::SetCameraPos( Vec2 pos )
{
	m_camera->SetPosition2D( pos );
}

// Camera Window
//////////////////////////////////////////////////////////////////////////
void CameraController::UpdateCameraWindow( float deltaSeconds )
{
	switch( m_owner->GetCameraWindowState() )
	{
	case NO_CAMERA_WINDOW:
		// Camera Pos Lock
		m_cameraWindowCenterPos = m_playerPos;
		m_cameraWindow.SetCenter( m_cameraWindowCenterPos );
		break;
	case USE_CAMERA_WINDOW:
		if( m_owner->GetCameraSnappingState() != NO_CAMERA_SNAPPING ) {
			Vec2 windowSnappedGoalPos = ComputeCameraWindowSnappedPosition( deltaSeconds );
 			m_cameraWindow.SetCenter( windowSnappedGoalPos );
		}
		if( !IsPointInsideAABB2D( m_playerPos, m_cameraWindow ) ) {
			m_cameraWindow.MoveToIncludePoint( m_playerPos );
		}
		break;
	}

	m_goalCameraPos = m_cameraWindow.GetCenter();

}

void CameraController::SetCameraWindowSize( Vec2 size )
{
	m_cameraWindow.SetDimensions( size );
}

void CameraController::BoundCameraPosInsideWindow()
{
	if( m_owner->GetCameraWindowState() == NO_CAMERA_WINDOW ){ return; }
	if( !m_cameraWindow.IsPointInside( m_smoothedGoalCameraPos ) ) {
		m_smoothedGoalCameraPos = m_cameraWindow.GetNearestPoint( m_smoothedGoalCameraPos );
	}
}

void CameraController::UpdateCameraShake( float deltaSeconds )
{
	if( m_trauma <= 0 ){ return; }
	m_trauma -= m_traumaDecaySpeed * deltaSeconds;
	m_trauma = ClampFloat( 0.f, 1.f, m_trauma );
	m_shakeIntensity = SmoothStop2( m_trauma );
	RandomNumberGenerator* rng = m_owner->GetRNG();
	Vec2 shakeDisp = Vec2::ZERO;
	float shakeRollDeg = 0.f;
	// pos shake
	shakeDisp = Vec2( rng->RollRandomFloatInRange( -1, 1 ), rng->RollRandomFloatInRange( -1, 1 ) );
	shakeDisp.Normalize();
	shakeDisp *= m_shakeIntensity * m_maxShakePosDist;

	// rot shake
	 shakeRollDeg = rng->RollRandomFloatInRange( -1, 1 );
	 shakeRollDeg *= m_shakeIntensity * m_maxShakeRotDeg;

	
	m_goalCameraPos += shakeDisp;
	m_camera->SetPitchRollYawRotation( 0.f, shakeRollDeg, 0.f );
}

void CameraController::UpdateCameraFrame( )
{
	if( m_owner->GetCameraFrameState() == NO_FRAMEING ) {
		return;
	}

	Vec2 fwdGoalPos		= m_playerPos;
	Vec2 projectGoalPos = m_playerPos;
	Vec2 cueGoalPos		= m_playerPos;

	// Update fwd framing
	if( m_player->IsContinousWalk() ) {
		Vec2 playerVelocity = m_player->GetVelocity();
		playerVelocity.Normalize();
		fwdGoalPos = m_playerPos + playerVelocity * m_fwdVelFocusDist;
	}

	// Update projectile framing
	Vec2 headDir = m_player->GetHeadDir();
	projectGoalPos = m_playerPos + m_aimFocusDist * headDir;

	// Update cue framing
	Vec2 cuePos = GetCuePos( );
	if( cuePos != Vec2::ZERO ) {
		Vec2 cueDisp = cuePos - m_playerPos;
		Vec3 cueDir = cueDisp.GetNormalized();
		cueGoalPos =  m_playerPos * 0.7f + cuePos * 0.3f;
	}

	// normalize
	float ratioSum = m_fwdVelFocusRatio + m_aimFocusRatio + m_cueFocusRatio;
	if( ratioSum == 0.f ) {
		//ERROR_RECOVERABLE( "ratio sum should not be zero" );
		return;
	}
	m_fwdVelFocusRatio	= m_fwdVelFocusRatio / ratioSum;
	m_aimFocusRatio		= m_aimFocusRatio / ratioSum;
	m_cueFocusRatio		= m_cueFocusRatio / ratioSum;

	m_goalCameraPos = ( m_fwdVelFocusRatio * fwdGoalPos ) + ( m_aimFocusRatio * projectGoalPos ) + ( m_cueFocusRatio * cueGoalPos );
}

void CameraController::SmoothMotion()
{
	m_smoothedGoalCameraPos = m_goalCameraPos;
	if( !m_isSmooth ){ 	return; }

	m_asymptoticValue = ComputeAsymptoticValueByDeltaDist( GetDistance2D( m_goalCameraPos, m_cameraPos ));
	m_smoothedGoalCameraPos = ( m_cameraPos * m_asymptoticValue ) + ( m_goalCameraPos * ( 1 - m_asymptoticValue ));
}

float CameraController::ComputeAsymptoticValueByDeltaDist( float deltaDist )
{
	float result =	RangeMapFloat( 0.f, m_maxDeltaDist, m_minAsymptotic, m_maxAsymptotic, deltaDist );
	result = ClampFloat( m_minAsymptotic, m_maxAsymptotic, result );
	return result;
}

void CameraController::UpdateMultipleCameraSettings( float deltaSeconds )
{
	if( !m_ismultipleFactorStable ) {
		UpdateMultipleCameraContributionRatio( deltaSeconds );
	}
}

void CameraController::UpdateMultipleCameraContributionRatio( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	float currentContributionSeconds = m_contributionRatioStableSeconds - (float)m_timer->GetSecondsRemaining();
	float smoothValue = 0.f;
	smoothValue = currentContributionSeconds / m_contributionRatioStableSeconds;
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );
	smoothValue = SmoothStep3( smoothValue );
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );

	m_currentMultipleCameraContributionsRatio = RangeMapFloat( 0.f, 1.f, m_startMultipleFactor, m_goalMultipleFactor, smoothValue ); // TODO testing
	m_currentMultipleCameraContributionsRatio = ClampFloat( 0.f, 1.f, m_currentMultipleCameraContributionsRatio );

	if( m_timer->HasElapsed() ) {
		m_ismultipleFactorStable = true;
		float time = (float)m_timer->m_clock->GetTotalSeconds();
		g_theConsole->DebugLogf( "elapsed, player: %d, elapsed: %.2f, start: %.2f, duration: %.2f", m_player->GetPlayerIndex(), time, m_timer->m_startSeconds, m_timer->m_durationSeconds );
		if( m_player->GetAliveState() == WAIT_FOR_DELETE ) {
			m_player->SetAliveState( AliveState::READY_TO_DELETE_CONTROLLER );
		}
	}
}

void CameraController::RenderToStencilTexture()
{
	if( m_owner->GetSplitScreenState() == NO_SPLIT_SCREEN ) { return; }
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
 	IntVec2 bufferSize = backBuffer->GetSize();
	m_stencilTexture = g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	g_theRenderer->CopyTexture( m_stencilTexture, s_stencilTexture );
	float rollDegrees = m_camera->m_transform.GetRollDegrees();
	m_camera->m_transform.SetRollDegrees( 0 );
	switch( m_owner->GetSplitScreenState() )
	{
		case AXIS_ALIGNED_SPLIT: {
 			AABB2 worldRenderBox  = GetWorldSpaceRenderBox(); // render area in client space but not revert y
 			m_camera->SetColorTarget( m_stencilTexture );
			g_theRenderer->BeginCamera( m_camera );
			g_theRenderer->BindShader( m_boxStencilShader );
			g_theRenderer->DrawAABB2DWithBound( worldRenderBox, Rgba8::WHITE, 0.5f, m_splitScreenEdgeColor );
			g_theRenderer->EndCamera();
		}
		break;
		case VORONOI_SPLIT: {
			m_camera->SetColorTarget( m_stencilTexture );
			g_theRenderer->BeginCamera( m_camera );
			g_theRenderer->BindShader( m_voronoiStencilShader );
			std::vector<Vec2> data;
			data.push_back( m_voronoiStencilOffset );
			data.push_back( Vec2::ZERO );
			m_voronoiOffsetBuffer->Update( data.data(), sizeof( Vec2 ) * 2, sizeof( Vec2 ) * 2 );
			g_theRenderer->SetOffsetBuffer( m_voronoiOffsetBuffer, 0 );
			AppendVertsForStencilTexture();
  			g_theRenderer->DrawVertexVector( m_stencilVertices );
			g_theRenderer->EndCamera();
		}
	}
	m_camera->m_transform.SetRollDegrees( rollDegrees );
}

void CameraController::AppendVertsForStencilTexture()
{
	AppendVertsForPolygon2D( m_stencilVertices, m_voronoiPolygon, Rgba8::WHITE );
	for( int i = 0; i <m_voronoiPolygon.m_edges.size(); i++ ) {
		float thickness = m_maxedgeThickness;
		LineSegment2 line = m_voronoiPolygon.GetEdgeInWorld( i );
		for( int j = 0; j < m_blendingEdgeAndThickness.size(); j++ ) {
			if( IsLineSeg2MostlyEqual( m_blendingEdgeAndThickness[j].first, line ) ) {
				thickness = RangeMapFloat( 0.f, 1.f, m_maxedgeThickness, 0.f, m_blendingEdgeAndThickness[j].second );
			}
		}
		LineSegment2 lineInworld = m_voronoiPolygon.GetEdgeInWorld( i );
		Vec2 lineDirt = lineInworld.GetNormalizedDirection();
		Vec2 moveDirt = Vec2( -lineDirt.y, lineDirt.x );
		lineInworld.SetStartPos( lineInworld.GetStartPos() + moveDirt * thickness * 0.45f );
		lineInworld.SetEndPos( lineInworld.GetEndPos() + moveDirt * thickness * 0.45f );
		AppendVertsForLineSegment2D( m_stencilVertices, lineInworld, thickness, m_splitScreenEdgeColor );
	}
}

void CameraController::RenderToTargetTexture()
{
	m_camera->SetColorTarget( m_colorTarget );
	g_theRenderer->BeginCamera( m_camera );
	if( m_owner->GetSplitScreenState() == VORONOI_SPLIT ) {
		g_theRenderer->BindShader( m_voronoiDiffuseShader );
		std::vector<Vec2> data;
		data.push_back( m_voronoiColorTargetOffset );
		data.push_back( Vec2::ZERO );
		m_voronoiOffsetBuffer->Update( data.data(), sizeof( Vec2 ) * 2, sizeof( Vec2 ) * 2 );
		g_theRenderer->SetOffsetBuffer( m_voronoiOffsetBuffer, 0 );
	}
	g_theGame->RenderGame( m_index );
	g_theRenderer->EndCamera();
}

void CameraController::UpdateMultipleCameraOffsetAndBuffer()
{
	if( m_owner->GetSplitScreenState() == NO_SPLIT_SCREEN ) {
		m_multipleCameraRenderOffset = Vec2::ZERO;
	}

	switch( m_owner->GetSplitScreenState() )
	{
		case AXIS_ALIGNED_SPLIT: {
			// controller num
			int controllerNum = m_owner->GetControllersNum();
			if( controllerNum == 1 ) {
				m_multipleCameraRenderOffset = Vec2::ZERO;
				return;
			}
			else if( controllerNum == 2 ) {
				float offsetX = ( 0.5f - m_currentMultipleCameraContributionsRatio / 2.f ) * 2.f;
				if( m_index == 0 ) {
					m_multipleCameraRenderOffset = Vec2( -offsetX, 0.f );
				}
				else if( m_index == 1 ) {
					m_multipleCameraRenderOffset = Vec2( offsetX, 0.f );
				}
			}
			else {
				if( m_index == 0 ) {
					m_multipleCameraRenderOffset = Vec2( -0.5f, 0.5f );
				}
				else if( m_index == 1 ) {
					m_multipleCameraRenderOffset = Vec2( 0.5f, 0.5f );
				}
				else if( m_index == 2 ) {
					m_multipleCameraRenderOffset = Vec2( -0.5f, -0.5f );
				}
				else if( m_index == 3 ) {
					m_multipleCameraRenderOffset = Vec2( 0.5f, -0.5f );
				}
			}
		}
		break;
		case VORONOI_SPLIT:{
			m_multipleCameraRenderOffset = Vec2::ZERO;
		}
		break;
		default:
		break;
	}
	std::vector<Vec2> data;
	data.push_back( m_multipleCameraRenderOffset );
	data.push_back( Vec2::ZERO );
	m_offsetBuffer->Update( data.data(), sizeof( Vec2 ) * 2, sizeof( Vec2 ) * 2 );
}

void CameraController::ReleaseRenderTarget()
{
	g_theRenderer->ReleaseRenderTarget( m_colorTarget );
	g_theRenderer->ReleaseRenderTarget( m_stencilTexture );
}

bool CameraController::IsPointInRenderArea( IntVec2 coords, IntVec2 textureSize, Polygon2 renderArea, AABB2 quickOutBox )
{
 	Vec2 coords_Vec2 = (Vec2)coords;

	return renderArea.IsPointInside( coords_Vec2 );
}

AABB2 CameraController::GetScreenRenderBox( IntVec2 size )
{
	AABB2 worldRenderBox = GetWorldSpaceRenderBox();
	AABB2 cameraWorldBox = g_gameCamera->GetWorldBox();
	AABB2 screenBox = AABB2( Vec2::ZERO, (Vec2)size );
	Vec2 screenRenderBoxMinUV = cameraWorldBox.GetUVForPoint( worldRenderBox.mins );
	Vec2 screenRenderBoxMaxUV = cameraWorldBox.GetUVForPoint( worldRenderBox.maxs );
	Vec2 screenRenderBoxMin	= screenBox.GetPointAtUV( screenRenderBoxMinUV );
	Vec2 screenRenderBoxMax	= screenBox.GetPointAtUV( screenRenderBoxMaxUV );
		
	return  AABB2( screenRenderBoxMin, screenRenderBoxMax );
}

AABB2 CameraController::GetWorldSpaceRenderBox()
{
	Vec2 boxDimension = GetSplitScreenBoxDimension();
	return AABB2( m_cameraPos, boxDimension.x, boxDimension.y );
}

Vec2 CameraController::GetSplitScreenBoxDimension()
{
	int ControllerNum = m_owner->GetControllersNum();
	float totalFactor = m_owner->GetTotalFactor();
	float totalWidth = GAME_CAMERA_MAX_X - GAME_CAMERA_MIN_X;
	float totalHeight = GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y;
	if( ControllerNum == 1 ) {
		return Vec2( totalWidth, totalHeight );
	}
	else if( ControllerNum == 2 ) {
		float width = totalWidth * ( m_currentMultipleCameraContributionsRatio / totalFactor );
		return Vec2( width, totalHeight );
	}
	else {
		float width = totalWidth / 2;
		float height = totalHeight / 2;
		return Vec2( width, height );
	}
}

Vec2 CameraController::GetMultipleCameraOffset()
{
	return Vec2::ZERO;
}

void CameraController::UpdateCameraPos()
{
	m_camera->SetPosition2D( m_smoothedGoalCameraPos );
}

float CameraController::GetMinimumInCircleRadiusForVoronoiHullWithPoint( Vec2 point )
{
	float minRadius = m_voronoiHull.m_planes[0].GetDistanceFromPlane( point );

	for( Plane2 plane : m_voronoiHull.m_planes ) {
		float radius = plane.GetDistanceFromPlane( point );
		if( radius < minRadius ) {
			minRadius = radius;
		}
	}
	return minRadius;
}

float CameraController::ComputeCameraSnapSpeed()
{
	float maxDist = m_cameraWindow.GetDiagonalLength() / 2;
 	float currentDist = GetDistance2D( m_cameraWindowCenterPos, m_playerPos );
 	return RangeMapFloat( 0.f, maxDist, m_snappingMinSpeed, m_snappingMaxSpeed, currentDist );
}

Vec2 CameraController::ComputeCameraWindowSnappedPosition( float deltaSeconds )
{
	if( m_owner->GetCameraSnappingState() == NO_CAMERA_SNAPPING ) {
		return m_cameraWindowCenterPos;
	}

	m_snappingSpeed = ComputeCameraSnapSpeed();
	Vec2 snapDirt = m_playerPos - m_cameraWindowCenterPos;
	switch( m_owner->GetCameraSnappingState() )
	{
	case POSITION_SNAPPING:
		break;
	case POSITION_VERTICAL_SNAPPING:
		snapDirt.x = 0.f;
		break;
	case POSITION_HORIZONTAL_SNAPPING:
		snapDirt.y = 0.f;
		break;
	}

	Vec2 snappedPos = Vec2::ZERO;
	if( snapDirt.GetLength() < 0.1f ) {
		snappedPos = m_cameraWindowCenterPos;
	}
	else {
		snapDirt.Normalize();
		snappedPos = m_cameraWindowCenterPos + ( deltaSeconds * m_snappingSpeed * snapDirt );
	}

	return snappedPos;
}

void CameraController::UpdateVoronoiPoly( PolyType type )
{
	std::vector<Vec2> polyPoints = m_voronoiHull.GetConvexPolyPoints();
	float minRadius;

	switch( type )
	{
		case ORIGINAL_POLY: {
			m_originalVoronoiPolygon		= Polygon2::MakeConvexFromPointCloud( polyPoints );
			m_originalVoronoiPolyArea		= m_originalVoronoiPolygon.GetArea();
			Vec2 polyCenter					= m_originalVoronoiPolygon.GetCenter();
			minRadius						= GetMinimumInCircleRadiusForVoronoiHullWithPoint( polyCenter );
			m_originalvoronoiInCircleRadius = minRadius;

		}
			break;
		case CURRENT_POLY: {
			m_voronoiPolygon			= Polygon2::MakeConvexFromPointCloud( polyPoints );
			m_voronoiPolyArea			= m_voronoiPolygon.GetArea();
			Vec2 polyCenter				= m_voronoiPolygon.GetCenter();
			minRadius					= GetMinimumInCircleRadiusForVoronoiHullWithPoint( polyCenter );
			m_voronoiInCircleRadius		= minRadius;
		}
		break;
		case INITIALIZE_POLY: {
			m_originalVoronoiPolygon		= Polygon2::MakeConvexFromPointCloud( polyPoints );
			m_originalVoronoiPolyArea		= m_originalVoronoiPolygon.GetArea();
			Vec2 polyCenter					= m_originalVoronoiPolygon.GetCenter();
			minRadius						= GetMinimumInCircleRadiusForVoronoiHullWithPoint( polyCenter );
			m_originalvoronoiInCircleRadius = minRadius;
			m_voronoiPolygon				= m_originalVoronoiPolygon;
			m_voronoiPolyArea				= m_originalVoronoiPolyArea;
			m_voronoiInCircleRadius			= m_originalvoronoiInCircleRadius;
		}
		break;
	}
}

void CameraController::UpdateVoronoiHull()
{
	std::vector<Vec2> voronoiPoints;
	m_voronoiPolygon.GetAllVerticesInWorld( voronoiPoints );
	ConvexPoly2 tempPoly = ConvexPoly2( voronoiPoints );
	m_voronoiHull = ConvexHull2::MakeConvexHullFromConvexPoly( tempPoly );
}

