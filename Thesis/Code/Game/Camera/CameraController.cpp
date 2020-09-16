#include "CameraController.hpp"
#include "Game/Player.hpp"
#include "Game/Camera/CameraSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext* g_theRenderer;


CameraController::CameraController( CameraSystem* owner, Player* player, Camera* camera )
	:m_owner( owner )
	,m_player( player )
	,m_camera( camera )
{

}

void CameraController::Update( float deltaSeconds )
{
	m_currentCameraPos = m_camera->GetPosition();
	UpdateCameraWindow( deltaSeconds );
	UpdateCamera();
	if( m_isDebug ) {
		DebugCameraInfo();
	}
}

void CameraController::DebugRender()
{
	if( m_isDebug ) {
		Vec2 windowPoints[4];
		m_cameraWindow.GetCornerPositions( windowPoints );
		for( int i = 0; i < 3; i++ ) {
			g_theRenderer->DrawLine( windowPoints[i], windowPoints[i + 1], 0.1f, Rgba8::BLUE );
		}
		g_theRenderer->DrawLine( windowPoints[0], windowPoints[3], 0.1f, Rgba8::BLUE );
	}
}

void CameraController::DebugCameraInfo()
{
	Vec2 playerPos = m_player->GetPosition();
	Vec2 cameraPos = m_camera->GetPosition();

	Strings debugStrings;

	std::string playerPosText = std::string( "Player pos =  " + playerPos.ToString() );
	std::string caneraPosText = std::string( "Camera pos = " + cameraPos.ToString() );

	debugStrings.push_back( playerPosText );
	debugStrings.push_back( caneraPosText );
	DebugAddScreenLeftAlignStrings( 0.5f, 0, Rgba8::WHITE, debugStrings );
}

void CameraController::SetIsDebug( bool isDebug )
{
	m_isDebug = isDebug;
}

// Camera Window
//////////////////////////////////////////////////////////////////////////
void CameraController::UpdateCameraWindow( float deltaSeconds )
{
	Vec2 playerPos = m_player->GetPosition();
	switch( m_owner->GetCameraWindowState() )
	{
	case NO_CAMERA_WINDOW:
		// Camera Pos Lock
		m_cameraWindowCenterPos = playerPos;
		m_cameraWindow.SetCenter( m_cameraWindowCenterPos );
		break;
	case USE_CAMERA_WINDOW:
		switch( m_owner->getcamera )
		{
		default:
			break;
		}

		if( !IsPointInsideAABB2D( playerPos, m_cameraWindow ) ) {
			m_cameraWindow.MoveToIncludePoint( playerPos );
			cameraPos = m_cameraWindow.GetCenter();
			m_camera->SetCenterPosition2D( cameraPos );
		}

		break;
	default:
		break;
	}
}

void CameraController::SetCameraWindowSize( Vec2 size )
{
	m_cameraWindow.SetDimensions( size );
}

void CameraController::UpdateCamera()
{
	m_camera->SetCenterPosition2D( m_goalCameraPos );
}

// void CameraController::SetCameraWindowMode( CameraWindowState newState )
// {
// 	m_cameraWindowState = newState;
// }
