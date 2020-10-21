#include "CameraController.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Game.hpp"
#include "Game/Player.hpp"
#include "Game/Camera/CameraSystem.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext*	g_theRenderer;
extern Game*			g_theGame;


CameraController::CameraController( CameraSystem* owner, Player* player, Camera* camera )
	:m_owner( owner )
	,m_player( player )
	,m_camera( camera )
{
	m_timer = new Timer();
}

CameraController::~CameraController()
{
	// leave map delete player
	// leave camera to future
	delete m_timer;
}

void CameraController::Update( float deltaSeconds )
{
	m_playerPos				= m_player->GetPosition();
	m_cameraPos				= m_camera->GetPosition();
	m_cameraWindowCenterPos	= m_cameraWindow.GetCenter();

	UpdateCameraWindow( deltaSeconds );
	UpdateCameraFrame( deltaSeconds );
	UpdateCameraShake( deltaSeconds );
	UpdateMultipleCameraSettings( deltaSeconds );
	SmoothMotion();
	BoundCameraPosInsideWindow();
	

	// Debug
// 	if( m_isDebug ) {
// 		DebugCameraInfo();
// 	}
}

void CameraController::DebugRender()
{
	if( m_isDebug ) {
		g_theRenderer->SetDiffuseTexture( nullptr );
		Vec2 windowPoints[4];
		m_cameraWindow.GetCornerPositions( windowPoints );
		for( int i = 0; i < 3; i++ ) {
			g_theRenderer->DrawLine( windowPoints[i], windowPoints[i + 1], 0.1f, Rgba8::BLUE );
		}
		g_theRenderer->DrawLine( windowPoints[0], windowPoints[3], 0.1f, Rgba8::BLUE );
		g_theRenderer->DrawCircle( m_goalCameraPos, 0.1f, 0.1f, Rgba8::BLACK );
		g_theRenderer->DrawCircle( m_smoothedGoalCameraPos, 0.1f, 0.1f, Rgba8::RED );
	}
}

void CameraController::DebugCameraInfoAt( Vec4 pos )
{
	Vec2 playerPos		= m_player->GetPosition();
	Vec2 cameraPos		= m_camera->GetPosition();
	Vec2 cameraGoalPos	= m_goalCameraPos;
	Strings debugStrings;
	float currentFactorSeconds = m_factorStableSeconds - m_timer->GetSecondsRemaining();

	std::string playerPosText					= std::string( "Player pos =  " + playerPos.ToString() );
	std::string cameraPosText					= std::string( "Camera pos = " + cameraPos.ToString() );
	std::string cameraGoalPosText				= std::string( "Goal Camera pos = " + cameraGoalPos.ToString() );
	std::string asymptoticValueText				= std::string( "Asymptotic Value = " + std::to_string( m_asymptoticValue ) );
	std::string traumaText						= std::string( "Trauma = " + std::to_string( m_trauma ) );
	std::string currentMultipleStableFactorText	= std::string( "multiple factor = " + std::to_string( m_currentMultipleFactor ));
	std::string goalMultipleStableFactorText	= std::string( "goal multiple factor = " + std::to_string( m_goalMultipleFactor ));
	std::string currentFactorSecondsText		= Stringf( "current factor seconds: %.2f", currentFactorSeconds );
	std::string totalFactorSecondsText			= Stringf( "total factor seconds: %.2f", m_factorStableSeconds );
	std::string testing							= std::to_string( (uint)m_player->GetAliveState() );

	debugStrings.push_back( playerPosText );
	debugStrings.push_back( cameraPosText );
	debugStrings.push_back( cameraGoalPosText );
	debugStrings.push_back( asymptoticValueText );
	debugStrings.push_back( traumaText );
	debugStrings.push_back( currentMultipleStableFactorText );
	debugStrings.push_back( goalMultipleStableFactorText );
	debugStrings.push_back( currentFactorSecondsText );
	debugStrings.push_back( totalFactorSecondsText );
	debugStrings.push_back( testing );
	//DebugAddScreenLeftAlignStrings( 0.15f, 0, Rgba8::WHITE, debugStrings );
	DebugAddScreenStrings( pos, Vec2::ZERO, 1.5f, Rgba8( 255, 255, 255, 125), debugStrings );
}

Vec2 CameraController::GetCuePos() const
{
	Map* currentMap = g_theGame->GetCurrentMap();
	return currentMap->GetCuePos();
}

void CameraController::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
}

void CameraController::SetIsSmooth( bool isSmooth )
{
	m_isSmooth = isSmooth;
}

void CameraController::SetAsymptoticValue( float value )
{
	m_asymptoticValue = value;
}

void CameraController::SetTrauma( float trauma )
{
	m_trauma = trauma;
}

void CameraController::AddTrauma( float addTrauma )
{
	m_trauma += addTrauma;
}

void CameraController::SetFwdFrameDist( float dist )
{
	m_fwdFrameVelDist = dist;
}

void CameraController::SetMultipleCameraStableFactorNotStableUntil( float totalSeconds, float goalFactor )
{
	m_ismultipleFactorStable = false;
	m_timer->SetSeconds( (double)totalSeconds );
	g_theConsole->DebugLogf( "set, player: %d, start: %.2f, duration: %.2f", m_player->GetPlayerIndex(), m_timer->m_startSeconds, m_timer->m_durationSeconds );
	m_startMultipleFactor = m_currentMultipleFactor;
	m_goalMultipleFactor = goalFactor;
	m_factorStableSeconds = totalSeconds;
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

	switch( m_owner->GetCameraShakeState() )
	{
	case POSITION_SHAKE:
		m_goalCameraPos += shakeDisp;
		break;
	case ROTATION_SHAKE: 
		m_camera->SetPitchRollYawRotation( 0.f, shakeRollDeg, 0.f );
		break;
	case BLEND_SHAKE:
		m_goalCameraPos += shakeDisp;
		m_camera->SetPitchRollYawRotation( 0.f, shakeRollDeg, 0.f );
		break;
	}
}

void CameraController::UpdateCameraFrame( float deltaSeconds )
{
	Vec2 fwdGoalPos = m_playerPos;
	Vec2 projectGoalPos = m_playerPos;
	Vec2 cueGoalPos = m_playerPos;

	// Update fwd framing
	if( m_player->IsContinousWalk() ) {
		Vec2 playerVelocity = m_player->GetVelocity();
		fwdGoalPos = m_playerPos + playerVelocity * m_fwdFrameVelDist;
	}

	// Update projectile framing
	Vec2 headDir = m_player->GetHeadDir();
	projectGoalPos = m_playerPos + m_projectFrameDist * headDir;

	// Update cue framing
	Vec2 cuePos = GetCuePos();
	if( cuePos != Vec2::ZERO ) {
		Vec2 cueDisp = cuePos - m_playerPos;
		Vec3 cueDir = cueDisp.GetNormalized();
		if( GetDistanceSquared2D( cuePos, m_playerPos ) < (m_cueFrameDist * m_cueFrameDist) ) {
			cueGoalPos = ( m_playerPos + cuePos ) / 2;
		}
		else{
			cueGoalPos = m_playerPos + cueDir * m_cueFrameDist;
		}
	}


	switch( m_owner->GetCameraFrameState() )
	{
	case NO_FRAMEING:
		return;
	case FORWARD_FRAMING:
		m_fwdFrameRatio = 1.f;
		m_projectFrameRatio = 0.f;
		m_cueFrameRatio = 0.f;
		break;
	case PROJECTILE_FRAMING:
		m_fwdFrameRatio = 0.f;
		m_projectFrameRatio = 1.f;
		m_cueFrameRatio = 0.f;
		break;
	case CUE_FRAMING:
		m_fwdFrameRatio = 0.f;
		m_projectFrameRatio = 0.f;
		m_cueFrameRatio = 1.f;
		break;
	case BLEND_FRAMING:
		m_fwdFrameRatio = 0.3f;
		m_projectFrameRatio = 0.3f;
		m_cueFrameRatio = 0.3f;
		break;
	}
	m_goalCameraPos = ( m_fwdFrameRatio * fwdGoalPos ) + ( m_projectFrameRatio * projectGoalPos ) + ( m_cueFrameRatio * cueGoalPos );
}

void CameraController::SmoothMotion()
{
	m_smoothedGoalCameraPos = m_goalCameraPos;
	if( !m_isSmooth ){ 	return; }

	m_asymptoticValue = ComputeAsymptoticValueByDeltaDist( GetDistance2D( m_smoothedGoalCameraPos, m_cameraPos ));
	m_smoothedGoalCameraPos = ( m_cameraPos * m_asymptoticValue ) + ( m_smoothedGoalCameraPos * ( 1 - m_asymptoticValue ));

}

float CameraController::ComputeAsymptoticValueByDeltaDist( float deltaDist )
{
	if( deltaDist == 0.f ) {
		return m_smoothRatio4;
	}

	float result =	RangeMapFloat( 0.f, m_maxDeltaDist, m_minAsymptotic, m_maxAsymptotic, deltaDist );
	result = ClampFloat( m_minAsymptotic, m_maxAsymptotic, result );
	return result;
}

void CameraController::UpdateMultipleCameraSettings( float deltaSeconds )
{
	if( !m_ismultipleFactorStable ) {
		UpdateMultipleCameraFactor( deltaSeconds );
	}
}

void CameraController::UpdateMultipleCameraFactor( float deltaSeconds )
{
	UNUSED(deltaSeconds);
	float currentFactorSeconds = m_factorStableSeconds - m_timer->GetSecondsRemaining();
	float smoothValue = 0.f;
	smoothValue = currentFactorSeconds / m_factorStableSeconds;
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );
	smoothValue = SmoothStep3( smoothValue );
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );

	m_currentMultipleFactor = RangeMapFloat( 0.f, 1.f, m_startMultipleFactor, m_goalMultipleFactor, smoothValue ); // TODO testing
	m_currentMultipleFactor = ClampFloat( 0.f, 1.f, m_currentMultipleFactor );

	if( m_timer->HasElapsed() ) {
		m_ismultipleFactorStable = true;
		float time = m_timer->m_clock->GetTotalSeconds();
		g_theConsole->DebugLogf( "elapsed, player: %d, elapsed: %.2f, start: %.2f, duration: %.2f", m_player->GetPlayerIndex(), time, m_timer->m_startSeconds, m_timer->m_durationSeconds );
// 		g_theConsole->DebugLogf( "start: %.2f", m_timer->m_startSeconds );
// 		g_theConsole->DebugLogf( "duration: %.2f", m_timer->m_durationSeconds );
		if( m_player->GetAliveState() == WAIT_FOR_DELETE ) {
			m_player->SetAliveState( AliveState::READY_TO_DELETE_CONTROLLER );
		}
	}
}

void CameraController::UpdateCameraPos()
{
	m_camera->SetPosition2D( m_smoothedGoalCameraPos );
}

float CameraController::ComputeCameraSnapSpeed()
{
	float maxDist = m_cameraWindow.GetDiagonalLength() / 2;
 	float currentDist = GetDistance2D( m_cameraWindowCenterPos, m_playerPos );
 	return RangeMapFloat( 0.f, maxDist, m_snappingMinSpeed, m_snappingMaxSpeed, currentDist );
}

Vec2 CameraController::ComputeCameraWindowSnappedPosition( float deltaSeconds )
{
	m_snappingSpeed = ComputeCameraSnapSpeed();
	Vec2 snapDirt = m_playerPos - m_cameraWindowCenterPos;
	Vec2 snappedPos =Vec2::ZERO;
	if( snapDirt.GetLength() < 0.1f ) {
		snappedPos = m_cameraWindowCenterPos;
	}
	else {
		snapDirt.Normalize();
		snappedPos = m_cameraWindowCenterPos + ( deltaSeconds * m_snappingSpeed * snapDirt );
	}

	switch( m_owner->GetCameraSnappingState() )
	{
	case POSITION_HORIZONTAL_LOCK:
		snappedPos.x = m_playerPos.x; 
		break;
	case POSITION_VERTICAL_LOCK:
		snappedPos.y = m_playerPos.y;
		break;
	case POSITION_LOCK:
		snappedPos = m_playerPos;
		break;
	case NO_CAMERA_SNAPPING:
		snappedPos = m_cameraWindowCenterPos;
		break;
	}
	return snappedPos;
}

