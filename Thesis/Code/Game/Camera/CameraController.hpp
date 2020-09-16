#pragma once
#include "Engine/Math/AABB2.hpp"

class Camera;
class Player;
class CameraSystem;

class CameraController {
	friend class Player;

public:
	CameraController(){}
	~CameraController(){}
	explicit CameraController( CameraSystem* owner, Player* player, Camera* camera );
	void Update( float deltaSeconds );
	void DebugRender();
	void DebugCameraInfo();

	// Accessor
	bool GetIsDebug() const { return m_isDebug; }

	// Mutator
	void SetIsDebug( bool isDebug );

	// Camera Window
	void UpdateCameraWindow( float deltaSeconds );
	void SetCameraWindowSize( Vec2 size );
	//void SetCameraWindowMode( CameraWindowState newState );
private:
	void UpdateCamera();

private:
	bool m_isDebug = false;
	Player* m_player = nullptr;
	Camera* m_camera = nullptr;
	CameraSystem* m_owner = nullptr;


	
	Vec2 m_goalCameraPos = Vec2::ZERO;
	Vec2 m_cameraWindowCenterPos = Vec2::ZERO;
	Vec2 m_currentCameraPos = Vec2::ZERO;
	// Camera Box
	//CameraWindowState m_cameraWindowState = NO_CAMERA_WINDOW;
	AABB2 m_cameraWindow;
};