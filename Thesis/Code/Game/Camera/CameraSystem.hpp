#pragma once
#include <vector>
#include "Game/Camera/CameraController.hpp"

class Player;
class Camera;

enum CameraWindowState: unsigned int {
	NO_CAMERA_WINDOW = 0,
	USE_CAMERA_WINDOW,
	NUM_OF_CAMERA_WINDOW_STATE,
};

enum CameraSnappingState : unsigned int {
	NO_CAMERA_SNAPPING = 0,
	POSITION_SNAPPING,
	POSITION_HORIZONTAL_LOCK,
	POSITION_VERTICAL_LOCK,
	POSITION_LOCK,
	NUM_OF_CAMERA_SNAPPING_STATE
};

class CameraSystem {
public:
	CameraSystem(){}
	~CameraSystem(){}

public:
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();
	void Update( float deltaSeconds );
	void UpdateControllers( float deltaSeconds );
	void DebugRender();
	void DebugRenderControllers();

	// Accessor
	bool GetIsDebug() const { return m_isdebug; }
	std::string GetCameraWindowStateText() const;

	// Mutator
	void SetIsDebug( bool isDebug );

	void UpdateDebugInfo();

	// Controller
	void CreateAndPushController( Player* player, Camera* camera );
	void SetCameraWindowState( CameraWindowState newState );
	CameraWindowState GetCameraWindowState() const { return m_cameraWindowState; }
	CameraSnappingState GetCameraSnappingState() const { return m_cameraSnappingState; }

private:
	bool m_isdebug = false;
	std::vector<CameraController*> m_controllers;
	CameraWindowState m_cameraWindowState		= NO_CAMERA_WINDOW;
	CameraSnappingState m_cameraSnappingState	= NO_CAMERA_SNAPPING;
};