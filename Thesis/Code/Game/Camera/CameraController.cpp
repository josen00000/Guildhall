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
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

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
	m_offsetBuffer = new RenderBuffer( "test", g_theRenderer, UNIFORM_BUFFER_BIT, MEMORY_HINT_DYNAMIC );
	m_boxShader		= g_theRenderer->GetOrCreateShader( "data/Shader/boxSplit.hlsl" );
	m_voronoiShader = g_theRenderer->GetOrCreateShader( "data/Shader/voronoiSplit.hlsl" );

	
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	int bufferSizeNum = backBuffer->GetHeight() * backBuffer->GetWidth();
	std::vector<Rgba8> stencilData( bufferSizeNum, Rgba8::BLACK );
	if( s_stencilTexture == nullptr ) {
		s_stencilTexture = g_theRenderer->CreateRenderTargetWithSizeAndData( backBuffer->GetSize(), stencilData.data() );
	}
}

CameraController::~CameraController()
{
	// leave map delete player
	// leave camera to future
	delete m_timer;
	SELF_SAFE_RELEASE(m_splitCamera);
	SELF_SAFE_RELEASE(m_offsetBuffer);


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

void CameraController::Render()
{
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
	m_colorTarget = g_theRenderer->AcquireRenderTargetMatching( backBuffer );

	UpdateStencilTexture();
	UpdateMultipleCameraOffset();
	std::vector<Vec2> data;
	data.push_back( m_multipleCameraRenderOffset );
	data.push_back( Vec2::ZERO );
	m_offsetBuffer->Update( data.data(), sizeof(Vec2) * 2, sizeof(Vec2) * 2 );
	Vec2 offset = GetMultipleCameraOffset();
	m_camera->SetColorTarget( m_colorTarget );
	g_theRenderer->BeginCamera( m_camera );
	g_theGame->RenderGame();
	g_theRenderer->EndCamera();
}

void CameraController::EndFrame()
{	
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
	float currentFactorSeconds = m_factorStableSeconds - (float)m_timer->GetSecondsRemaining();


	std::string playerPosText					= std::string( "Player pos =  " + playerPos.ToString() );
	std::string cameraPosText					= std::string( "Camera pos = " + cameraPos.ToString() );
	std::string cameraGoalPosText				= std::string( "Goal Camera pos = " + cameraGoalPos.ToString() );
	std::string asymptoticValueText				= std::string( "Asymptotic Value = " + std::to_string( m_asymptoticValue ) );
	std::string traumaText						= std::string( "Trauma = " + std::to_string( m_trauma ) );
	std::string currentMultipleStableFactorText	= std::string( "multiple factor = " + std::to_string( m_currentMultipleFactor ));
	std::string goalMultipleStableFactorText	= std::string( "goal multiple factor = " + std::to_string( m_goalMultipleFactor ));
	std::string currentFactorSecondsText		= Stringf( "current factor seconds: %.2f", currentFactorSeconds );
	std::string totalFactorSecondsText			= Stringf( "total factor seconds: %.2f", m_factorStableSeconds );
	std::string testing							= std::to_string( m_testDuration.count() );


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

void CameraController::SetVoronoiPolygon( Polygon2 poly )
{
	m_voronoiPolygon = poly;
}

void CameraController::SetVoronoiOffset( Vec2 offset )
{
	m_voronoiOffset = offset;
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
	UNUSED(deltaSeconds);

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
	float currentFactorSeconds = m_factorStableSeconds - (float)m_timer->GetSecondsRemaining();
	float smoothValue = 0.f;
	smoothValue = currentFactorSeconds / m_factorStableSeconds;
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );
	smoothValue = SmoothStep3( smoothValue );
	smoothValue = ClampFloat( 0.f, 1.f, smoothValue );

	m_currentMultipleFactor = RangeMapFloat( 0.f, 1.f, m_startMultipleFactor, m_goalMultipleFactor, smoothValue ); // TODO testing
	m_currentMultipleFactor = ClampFloat( 0.f, 1.f, m_currentMultipleFactor );

	if( m_timer->HasElapsed() ) {
		m_ismultipleFactorStable = true;
		float time = (float)m_timer->m_clock->GetTotalSeconds();
		g_theConsole->DebugLogf( "elapsed, player: %d, elapsed: %.2f, start: %.2f, duration: %.2f", m_player->GetPlayerIndex(), time, m_timer->m_startSeconds, m_timer->m_durationSeconds );
// 		g_theConsole->DebugLogf( "start: %.2f", m_timer->m_startSeconds );
// 		g_theConsole->DebugLogf( "duration: %.2f", m_timer->m_durationSeconds );
		if( m_player->GetAliveState() == WAIT_FOR_DELETE ) {
			m_player->SetAliveState( AliveState::READY_TO_DELETE_CONTROLLER );
		}
	}
}

int CameraController::GetTextureIndexWithScreenCoords( int x, int y, IntVec2 textureSize )
{
	y = textureSize.y - 1 - y;
	return x + ( textureSize.x * y );
}

IntVec2 CameraController::GetTextureCoordsWithScreenCoords( int x, int y, IntVec2 textureSize )
{
	y = textureSize.y - 1 - y;
	return IntVec2( x, y );
}

void CameraController::UpdateStencilTexture()
{
	if( m_owner->GetSplitScreenState() == NO_SPLIT_SCREEN ) { return; }
	Texture* backBuffer = g_theRenderer->GetSwapChainBackBuffer();
 	IntVec2 bufferSize = backBuffer->GetSize();
	m_stencilTexture = g_theRenderer->AcquireRenderTargetMatching( backBuffer );
	g_theRenderer->CopyTexture( m_stencilTexture, s_stencilTexture );

	switch( m_owner->GetSplitScreenState() )
	{
		case AXIS_ALIGNED_SPLIT: {
 			AABB2 worldRenderBox  = GetWorldSpaceRenderBox(); // render area in client space but not revert y
 			m_camera->SetColorTarget( m_stencilTexture );
			g_theRenderer->BeginCamera( m_camera );
			g_theRenderer->BindShader( m_boxShader );
			g_theRenderer->DrawAABB2D( worldRenderBox, Rgba8::RED );
			g_theRenderer->EndCamera();
		}
		break;
		case VORONOI_SPLIT: {
			m_camera->SetColorTarget( m_stencilTexture );
			g_theRenderer->BeginCamera( m_camera );
			g_theRenderer->BindShader( m_boxShader );
			g_theRenderer->DrawPolygon2D( m_voronoiPolygon, Rgba8::RED );
			g_theRenderer->EndCamera();
		}
	}
}

void CameraController::UpdateMultipleCameraOffset()
{
	if( m_owner->GetSplitScreenState() == NO_SPLIT_SCREEN ) {
		m_multipleCameraRenderOffset = Vec2::ZERO;
		return;
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
				if( m_index == 0 ) {
					m_multipleCameraRenderOffset = Vec2( -0.5f, 0.f );
				}
				else if( m_index == 1 ) {
					m_multipleCameraRenderOffset = Vec2( 0.5f, 0.f );
				}
				else {
					ERROR_RECOVERABLE(" should not exist third controller" );
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
			AABB2 splitCheckBox = m_owner->GetSplitCheckBox();
			AABB2 worldCameraBox = m_owner->GetWorldCameraBox();
			Vec2 posInSplitCheckBox = splitCheckBox.GetNearestPoint( m_cameraPos );
			Vec2 disp = posInSplitCheckBox - splitCheckBox.GetCenter();
			Vec2 dispInScreenSpace;
			dispInScreenSpace.x = disp.x / splitCheckBox.GetDimensions().x;
			dispInScreenSpace.y = disp.y / splitCheckBox.GetDimensions().y;
			float xRatio = splitCheckBox.GetWidth() / worldCameraBox.GetWidth();
			float yRatio = splitCheckBox.GetHeight() / worldCameraBox.GetHeight();
			dispInScreenSpace.x *= xRatio;
			dispInScreenSpace.y *= yRatio;
			m_multipleCameraRenderOffset = dispInScreenSpace * 2;
			//m_multipleCameraRenderOffset = Vec2::ZERO;
		}
		break;
		default:
		break;
	}
}

void CameraController::ReleaseRenderTarget()
{
	g_theRenderer->ReleaseRenderTarget( m_colorTarget );
	g_theRenderer->ReleaseRenderTarget( m_stencilTexture );
}

bool CameraController::IsPointInRenderArea( IntVec2 coords, IntVec2 textureSize, Polygon2 renderArea, AABB2 quickOutBox )
{
 	Vec2 coords_Vec2 = (Vec2)coords;
// 	if( !quickOutBox.IsPointInside( coords_Vec2 ) ){ return false; }

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
	Vec2 boxDimension = GetSPlitScreenBoxDimension();
	return AABB2( m_cameraPos, boxDimension.x, boxDimension.y );
}

Vec2 CameraController::GetSPlitScreenBoxDimension()
{
	int ControllerNum = m_owner->GetControllersNum();
	float totalFactor = m_owner->GetTotalFactor();
	float totalWidth = GAME_CAMERA_MAX_X - GAME_CAMERA_MIN_X;
	float totalHeight = GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y;
	if( ControllerNum == 1 ) {
		return Vec2( totalWidth, totalHeight );
	}
	else if( ControllerNum == 2 ) {
		float width = totalWidth * ( m_currentMultipleFactor / totalFactor );
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

