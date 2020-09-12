#include "CameraSystem.hpp"
#include "Engine/Renderer/DebugRender.hpp"

void CameraSystem::Startup()
{
	m_controllers.reserve( 4 );
}

void CameraSystem::Shutdown()
{

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
	if( m_isdebug ) {
		UpdateDebugInfo();
	}
}

void CameraSystem::UpdateControllers( float deltaSeconds )
{
	for( int i = 0; i < m_controllers.size(); i++ ) {
		m_controllers[i]->Update( deltaSeconds );
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
	case NUM_OF_CAMERA_WINDOW:
		return result +std::string( "Num of Camera window" );
	}
	return "";
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
	std::string windowStateText = GetCameraWindowStateText();
	Strings debugInfos;
	debugInfos.push_back( windowStateText );
	DebugAddScreenLeftAlignStrings( 0.8, 0.f, Rgba8::RED, debugInfos );
}

void CameraSystem::CreateAndPushController( Player* player, Camera* camera )
{
	CameraController* tempCamController = new CameraController( this, player, camera );
	tempCamController->SetCameraWindowSize( Vec2( 12, 8 ) );
	m_controllers.push_back( tempCamController );
}

void CameraSystem::SetCameraWindowState( CameraWindowState newState )
{
	m_cameraWindowState = newState;
}

