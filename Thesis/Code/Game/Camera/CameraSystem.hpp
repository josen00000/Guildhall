#pragma once
#include <vector>
#include "Game/Camera/CameraController.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map/Map.hpp"

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

enum SplitScreenState : unsigned int {
	NO_SPLIT_SCREEN = 0,
	NUM_OF_SPLIT_SCREEN_STATE,
};

enum NoSplitScreenStrat : unsigned int {
	NO_STRAT = 0,
	ZOOM_TO_FIT,
	KILL_AND_TELEPORT,
	NUM_OF_NO_SPLIT_SCREEN_STRAT
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
	void UpdateSplitScreenEffects( float deltaSeconds );
	void UpdateNoSplitScreenEffect( float deltaSeconds );
	void UpdateControllerCameras();
	void DebugRender();
	void DebugRenderControllers();

	// Accessor
	bool GetIsDebug() const { return m_isdebug; }
	std::string GetCameraWindowStateText() const;
	std::string GetCameraSnappingStateText() const;
	std::string GetCameraShakeStateText() const;
	std::string GetCameraFrameStateText() const;
	std::string GetSplitScreenStateText() const;
	std::string GetNoSplitScreenStratText() const;

	CameraWindowState	GetCameraWindowState() const { return m_cameraWindowState; }
	CameraSnappingState GetCameraSnappingState() const { return m_cameraSnappingState; }
	CameraShakeState	GetCameraShakeState() const { return m_cameraShakeState; }
	CameraFrameState	GetCameraFrameState() const { return m_cameraFrameState; }
	SplitScreenState	GetSplitScreenState() const { return m_splitScreenState; }
	NoSplitScreenStrat	GetNoSplitScreenStrat() const { return m_noSplitScreenStrat; }

	std::vector<CameraController*>& GetCameraControllers();
	RandomNumberGenerator* GetRNG(){ return m_rng; }

	// Mutator
	void SetIsDebug( bool isDebug );
	void SetCameraWindowState( CameraWindowState newState );
	void SetCameraSnappingState( CameraSnappingState newState );
	void SetCameraShakeState( CameraShakeState newState );
	void SetCameraFrameState( CameraFrameState newState );
	void SetSplitScreenState( SplitScreenState newState );
	void SetNoSplitScreenStrat( NoSplitScreenStrat newStrat ); 
	void SetMap( Map* map );

	void AddCameraShake( int index, float shakeTrauma );

	void UpdateDebugInfo();

	// Controller
	void CreateAndPushController( Player* player, Camera* camera );

	// split screen strat
		// no split screen strat
	bool IsControllersInsideCamera();
	void GetNotInsideControllers( std::vector<CameraController*>& vec, float insidePaddingLength );
	bool CheckIfZoomStable( float stablePaddingLength );
	void KillAndTeleportPlayers( std::vector<CameraController*>& vec );
	void ZoomCameraToFitPlayers();
	float ComputeCameraHeight( std::vector<CameraController*> const& vec );

private:
	bool m_isdebug = false;
	std::vector<CameraController*> m_controllers;
	
	// states
	CameraWindowState	m_cameraWindowState		= NO_CAMERA_WINDOW;
	CameraSnappingState m_cameraSnappingState	= NO_CAMERA_SNAPPING;
	CameraShakeState	m_cameraShakeState		= POSITION_SHAKE;
	CameraFrameState	m_cameraFrameState		= NO_FRAMEING;
	SplitScreenState	m_splitScreenState		= NO_SPLIT_SCREEN;
	NoSplitScreenStrat	m_noSplitScreenStrat	= NO_STRAT;
	RandomNumberGenerator* m_rng	= nullptr;

	float m_shakeBlendAmount	= 0.f;

	// no split settings
	bool m_isCameraZoomStable				= false;
	int m_recoverWaitFrame					= 10;
	float m_notInsidePaddingLength			= 3.f;
	float m_zoomStablePaddingLength			= 4.f;
	float m_idealCameraHeight				= GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y;
	float m_maxCameraDeltaHeightPerFrame	= 0.01f;
	Vec2 m_noSplitCameraSize				= Vec2( ( GAME_CAMERA_MAX_X - GAME_CAMERA_MIN_X ), ( GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y) );


	Vec2  m_goalCameraPos		= Vec2::ZERO;
	AABB2 m_goalCameraWindow	= AABB2();

	Camera* m_noSplitCamera	= nullptr;
	Map*	m_map			= nullptr;
	//Timer*
};