#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include <chrono>

class Camera;
class Player;
class CameraSystem;
class Timer;
class Texture;
struct Vec4;
class RenderBuffer;
class Shader;

class CameraController {
	friend class Player;
	friend class CameraSystem;

public:
	CameraController(){}
	~CameraController();
	explicit CameraController( CameraSystem* owner, Player* player, Camera* camera );
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();
	void DebugRender();
	void DebugCameraInfoAt( Vec4 pos );


	// Accessor
	int GetIndex() const { return m_index; }
	bool GetIsDebug() const { return m_isDebug; }
	float GetCurrentMultipleFactor() const { return m_currentMultipleFactor; }
	Vec2 GetCuePos() const ;
	Vec2 GetSmoothedGoalPos() const { return m_smoothedGoalCameraPos; }
	Vec2 GetCameraPos() const { return m_cameraPos; }
	Polygon2 GetVoronoiPoly() const { return m_voronoiPolygon; }

	Texture* GetColorTarget() { return m_colorTarget; }
	Texture* GetStencilTexture() { return m_stencilTexture; }
	RenderBuffer* GetOffsetBuffer(){ return m_offsetBuffer; }

	Camera* GetCamera(){ return m_camera; }

	// Mutator
	void SetIndex( int index );
	void SetIsDebug( bool isDebug );
	void SetIsSmooth( bool isSmooth );
	void SetAsymptoticValue( float value );
	void SetTrauma( float trauma );
	void AddTrauma( float addTrauma );
	void SetFwdFrameDist( float dist );
	void SetMultipleCameraStableFactorNotStableUntil( float totalSeconds, float goalFactor );
	void SetVoronoiPolygon( Polygon2 poly );
	void SetVoronoiOffset( Vec2 offset );

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

	// split screen
	int GetTextureIndexWithScreenCoords( int x, int y, IntVec2 textureSize );	   // with world space coords
	IntVec2 GetTextureCoordsWithScreenCoords( int x, int y, IntVec2 textureSize ); // with world space coords

	void UpdateStencilTexture();
	void UpdateMultipleCameraOffset();
	void ReleaseRenderTarget();
	
	bool IsPointInRenderArea( IntVec2 coords, IntVec2 textureSize, Polygon2 renderArea, AABB2 quickOutBox );
	AABB2 GetScreenRenderBox( IntVec2 size );
	AABB2 GetWorldSpaceRenderBox();
	Vec2 GetSPlitScreenBoxDimension();
	Vec2 GetMultipleCameraOffset();

	void UpdateCameraPos();
private:

	// camera snapping
	float ComputeCameraSnapSpeed();
	Vec2 ComputeCameraWindowSnappedPosition( float deltaSeconds );

public:
	static Texture* s_stencilTexture;

private:
	int m_index						= 0;
	bool m_isDebug					= false;
	bool m_disableUpdateCamera		= false;
	Player* m_player				= nullptr; // const
	Camera* m_camera				= nullptr;
	Camera* m_splitCamera			= nullptr;
	Texture* m_stencilTexture		= nullptr;
	Texture* m_colorTarget			= nullptr;
	Shader* m_boxShader				= nullptr;
	Shader* m_voronoiShader			= nullptr;

	CameraSystem* m_owner		= nullptr;
	Timer*	m_timer				= nullptr;

	Vec2 m_playerPos					= Vec2::ZERO;
	Vec2 m_goalCameraPos				= Vec2::ZERO;
	Vec2 m_smoothedGoalCameraPos		= Vec2::ZERO;
	Vec2 m_cameraPos					= Vec2::ZERO;
	Vec2 m_cameraWindowCenterPos		= Vec2::ZERO;
	Vec2 m_multipleCameraRenderOffset	= Vec2::ZERO;

	// Camera winodw
	AABB2 m_cameraWindow;
	
	// Camera snapping
	float m_snappingMinSpeed	= 2.f;
	float m_snappingMaxSpeed	= 4.f;
	float m_snappingSpeed		= 0.f;

	// camera smooth
	bool m_isSmooth				= true;
	float m_asymptoticValue		= 0.9f;
	float m_maxDeltaDist		= 5.f;
	float m_minAsymptotic		= 0.95f;
	float m_maxAsymptotic		= 0.99f;

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
	float m_goalMultipleFactor		= 1.f;
	float m_startMultipleFactor		= 0.f;
	float m_currentMultipleFactor	= 0.f;
	float m_factorStableSeconds		= 0.f; 

	RenderBuffer* m_offsetBuffer	= nullptr;
	Vec2	m_voronoiOffset = Vec2::ZERO;
	Polygon2 m_voronoiPolygon;

	std::chrono::microseconds m_testDuration;
};