#pragma once
#include "Engine/Math/AABB2.hpp"

class Camera;
class Player;
class CameraSystem;
class Timer;

class CameraController {
	friend class Player;
	friend class CameraSystem;

public:
	CameraController(){}
	~CameraController(){}
	explicit CameraController( CameraSystem* owner, Player* player, Camera* camera );
	void Update( float deltaSeconds );
	void DebugRender();
	void DebugCameraInfo();

	// Accessor
	bool GetIsDebug() const { return m_isDebug; }
	Vec2 GetCuePos() const ;
	Vec2 GetSmoothedGoalPos() const { return m_smoothedGoalCameraPos; }

	// Mutator
	void SetIsDebug( bool isDebug );
	void SetAsymptoticValue( float value );
	void SetTrauma( float trauma );
	void AddTrauma( float addTrauma );
	void SetFwdFrameDist( float dist );
	void SetMultipleCameraFactorNotStableUntil( float totalSeconds );

	// Camera Window
	void UpdateCameraWindow( float deltaSeconds );
	void SetCameraWindowSize( Vec2 size );
	void BoundCameraPosInsideWindow();
	//void SetCameraWindowMode( CameraWindowState newState );

	// Shake
	void UpdateCameraShake( float deltaSeconds );

	// Camera Framing
	void UpdateCameraFrame( float deltaSeconds );
	void SmoothMotion();
	float ComputeAsymptoticValueByDeltaDist( float deltaDist );

	// Multiple camera
	void UpdateMultipleCameraSettings( float deltaSeconds );
	void UpdateMultipleCameraFactor( float deltaSeconds );

	void UpdateCameraPos();
private:

	// camera snapping
	float ComputeCameraSnapSpeed();
	Vec2 ComputeCameraWindowSnappedPosition( float deltaSeconds );

private:
	bool m_isDebug				= false;
	bool m_disableUpdateCamera	= false;
	Player* m_player = nullptr;
	Camera* m_camera = nullptr;
	CameraSystem* m_owner = nullptr;
	Timer*	m_timer = nullptr;

	Vec2 m_playerPos				= Vec2::ZERO;
	Vec2 m_goalCameraPos			= Vec2::ZERO;
	Vec2 m_smoothedGoalCameraPos	= Vec2::ZERO;
	Vec2 m_cameraPos				= Vec2::ZERO;
	Vec2 m_cameraWindowCenterPos	= Vec2::ZERO;

	// Camera winodw
	AABB2 m_cameraWindow;
	
	// Camera snapping
	float m_snappingMinSpeed	= 2.f;
	float m_snappingMaxSpeed	= 4.f;
	float m_snappingSpeed		= 0.f;

	// camera smooth
	float m_asymptoticValue		= 0.9f;
	float m_maxDeltaDist		= 5.f;
	float m_minAsymptotic		= 0.95f;
	float m_maxAsymptotic		= 0.99f;
	int m_smoothStep			= 0;

	// Camera Shake
	float m_trauma				= 0.f;
	float m_traumaDecaySpeed	= 1.f;
	float m_shakeIntensity		= 0.f;
	float m_maxShakePosDist		= 1.f;
	float m_maxShakeRotDeg		= 10.f;
	float m_shakePosRatio		= 0.f;
	float m_shakeRotRatio		= 0.f;

	// Camera framing
	float m_fwdFrameVelDist		= 1.5f;
	float m_projectFrameDist	= 1.5f;
	float m_cueFrameDist		= 1.5f;
	float m_fwdFrameMaxDist		= 3.f;
	float m_smoothRatio4		= 0.90f;
	float m_fwdFrameRatio		= 0.f;
	float m_projectFrameRatio	= 0.f;
	float m_cueFrameRatio		= 0.f;

	// multiple camera
	bool m_ismultipleFactorStable	= true;
	float m_multipleFactor			= 0.01f;
	float m_factorStableSeconds		= 0.f; 
};