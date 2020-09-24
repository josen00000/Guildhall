#pragma once
#include <vector>
#include "Game/Camera/CameraController.hpp"

class Player;
class Camera;
class RandomNumberGenerator;

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

enum CameraShakeState : unsigned int {
	POSITION_SHAKE = 0,
	ROTATION_SHAKE,
	BLEND_SHAKE,
	NUM_OF_SHAKE_STATE
};

enum CameraFrameState : unsigned int {
	NO_FRAMEING = 0,
	FORWARD_FRAMING,
	PROJECTILE_FRAMING,
	CUE_FRAMING,
	BLEND_FRAMING,
	NUM_OF_FRAME_STATE
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
	std::string GetCameraSnappingStateText() const;
	std::string GetCameraShakeStateText() const;
	std::string GetCameraFrameStateText() const;
	CameraWindowState	GetCameraWindowState() const { return m_cameraWindowState; }
	CameraSnappingState GetCameraSnappingState() const { return m_cameraSnappingState; }
	CameraShakeState	GetCameraShakeState() const { return m_cameraShakeState; }
	CameraFrameState	GetCameraFrameState() const { return m_cameraFrameState; }
	std::vector<CameraController*>& GetCameraControllers();
	RandomNumberGenerator* GetRNG(){ return m_rng; }

	// Mutator
	void SetIsDebug( bool isDebug );
	void SetCameraWindowState( CameraWindowState newState );
	void SetCameraSnappingState( CameraSnappingState newState );
	void SetCameraShakeState( CameraShakeState newState );
	void SetCameraFrameState( CameraFrameState newState );
	void AddCameraShake( int index, float shakeTrauma );

	void UpdateDebugInfo();

	// Controller
	void CreateAndPushController( Player* player, Camera* camera );

private:
	bool m_isdebug = false;
	std::vector<CameraController*> m_controllers;
	
	// states
	CameraWindowState	m_cameraWindowState		= NO_CAMERA_WINDOW;
	CameraSnappingState m_cameraSnappingState	= NO_CAMERA_SNAPPING;
	CameraShakeState	m_cameraShakeState		= POSITION_SHAKE;
	CameraFrameState	m_cameraFrameState		= NO_FRAMEING;
	RandomNumberGenerator* m_rng	= nullptr;
	float m_shakeBlendAmount	= 0.f;
};