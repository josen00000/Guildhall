#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Polygon2.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include <chrono>

class Camera;
class Player;
class CameraSystem;
class Timer;
class Texture;
struct Vec4;
class RenderBuffer;
class Shader;

typedef std::pair<LineSegment2, float> edgeAndThickness;

enum PolyType {
	ORIGINAL_POLY,
	CURRENT_POLY,
	INITIALIZE_POLY,
};

class CameraController {
	friend class Player;
	friend class CameraSystem;

public:
	CameraController(){}
	~CameraController();
	explicit CameraController( CameraSystem* owner, Player* player, Camera* camera );
	void BeginFrame();
	void Update( float deltaSeconds );
	void Render();
	void EndFrame();
	void DebugRender();
	void DebugCameraInfoAt( Vec4 pos );

	// Accessor
	int GetIndex() const								{ return m_index; }
	bool GetNeedRenderWhenMerged() const				{ return m_needRenderWhenMerged; }
	bool GetIsDebug() const								{ return m_isDebug; }
	bool GetAllowMerge() const							{ return m_allowMerge; }
	bool GetAllowBlend() const							{ return m_allowBlend; }
	float GetCurrentMultipleFactor() const				{ return m_currentMultipleCameraContributionsRatio; }
	float GetVoronoiArea() const						{ return m_voronoiPolyArea; }
	float GetOriginalVoronoiArea() const				{ return m_originalVoronoiPolyArea; }
	float GetVoronoiInCircleRadius() const				{ return m_voronoiInCircleRadius; }
	float GetOriginalVoronoiInCircleRadius() const		{ return m_originalvoronoiInCircleRadius; }
	float GetForwardVelocityCameraFrameDist() const		{ return m_fwdVelFocusDist; }
	float GetAimCameraFrameDist() const					{ return m_aimFocusDist; }
	float GetForwardVelocityCameraFrameRatio() const	{ return m_fwdVelFocusRatio; }
	float GetAimCameraFrameRatio() const				{ return m_aimFocusRatio; }
	float GetCueCameraFrameRatio() const				{ return m_cueFocusRatio; }
	float GetTrauma() const								{ return m_trauma; }

	Vec2 GetCameraWindowSize() const				{ return m_cameraWindow.GetDimensions(); }
	Vec2 GetCuePos() const ;
	Vec2 GetSmoothedGoalPos() const					{ return m_smoothedGoalCameraPos; }
	Vec2 GetCameraPos() const						{ return m_cameraPos; }
	Vec2 GetVoronoiOffset() const					{ return m_voronoiStencilOffset; }
	Vec2 GetVoronoiAnchorPointPos() const			{ return m_voronoiAnchorPointPos; }
	Vec2 GetOriginalVoronoiAnchorPointPos() const	{ return m_originalVoronoiAnchorPointPos; }
	Vec2 GetTargetVoronoiAnchorPointPos() const		{ return m_targetVoronoiAnchorPointPos; }
	Vec2 GetVoronoiColortargetOffset() const		{ return m_voronoiColorTargetOffset; }

	Polygon2 const& GetVoronoiPolygon() const;
	Polygon2 const& GetOriginalVoronoiPolygon() const;
	ConvexHull2 GetVoronoiHull() const				{ return m_voronoiHull; }

	Texture* GetColorTarget()						{ return m_colorTarget; }
	Texture* GetStencilTexture()					{ return m_stencilTexture; }
	RenderBuffer* GetOffsetBuffer()					{ return m_offsetBuffer; }

	Camera* GetCamera(){ return m_camera; }

	// Mutator
	void SetIndex									( int index );
	void SetIsDebug									( bool isDebug );
	void SetIsSmooth								( bool isSmooth );
	void SetAllowMerge								( bool allowMerge );
	void SetAllowBlend								( bool allowBlend );
	void SetNeedRenderWhenMerged					( bool doesNeedRenderWhenMerged );

	void SetAsymptoticValue							( float value );
	void SetMaxAsymptoticValue						( float maxValue );
	void SetMinAsymptoticValue						( float minValue );
	void SetMaxDeltaDistance						( float maxDeltaDist );
	void SetTrauma									( float trauma );
	void AddTrauma									( float addTrauma );

	// camera frame
	void SetUseCameraFrame							( bool useCameraFrame );
	void SetForwardVelocityFocusDist				( float dist );
	void SetAimFocusDist							( float dist );
	void SetForwardVelocityFocusRatio				( float ratio );
	void SetAimFocusRatio							( float ratio );
	void SetCueFocusRatio							( float ratio );
	void SetPositionalShakeMaxDist					( float maxDist );
	void SetRotationalShakeMaxDeg					( float maxDeg );
	void SetMaxEdgeThickness						( float maxThickness );
	void SetSplitScreenEdgeColor					( Rgba8 color ); 

	void SetMultipleCameraStableFactorNotStableUntil( float totalSeconds, float goalFactor );

	void SetCameraPos								( Vec2 pos );
	void SetVoronoiAnchorPointPos					( Vec2 voronoiAnchorPoint );
	void SetCameraWindowSize						( Vec2 size );
	void SetMultipleCameraRenderOffset				( Vec2 offset );
	void SetVoronoiStencilOffset					( Vec2 offset );
	void SetVoronoiColorTargetOffset				( Vec2 offset );
	void SetTargetVoronoiAnchorPointPos				( Vec2 targetVoronoiAnchorPoint );
	void SetOriginalVoronoiAnchorPointPos			( Vec2 smoothedVoronoiAnchorPoint );
	bool ReplaceVoronoiPointWithPoint				( Vec2 replacedPoint,  Vec2 newPoint );

	void SetVoronoiPolygon							( Polygon2 const & poly );
	void SetOriginalVoronoiPolygon					( Polygon2 const & poly );
	void SetVoronoiHullAndUpdateVoronoiPoly			( ConvexHull2 const& hull, PolyType type  );
	void AddPlaneToVoronoiHull						( Plane2 const& plane, PolyType type );
	void addBlendingEdgeIndexAndThickness			( LineSegment2 const& line, float thicknessRatio );

	// Camera Window
	void UpdateCameraWindow							( float deltaSeconds );
	void BoundCameraPosInsideWindow();

	// Shake
	void UpdateCameraShake							( float deltaSeconds );

	// Camera Frame
	void UpdateCameraFrame							();

	// Camera smooth
	void SmoothMotion								( float deltaSeconds );
	float ComputeAsymptoticValueByDeltaDist			( float deltaDist );

	// Multiple camera
	void UpdateMultipleCameraSettings				( float deltaSeconds );
	void UpdateMultipleCameraContributionRatio		( float deltaSeconds );

	// split screen
	void RenderToStencilTexture();
	void AppendVertsForStencilTexture();
	void RenderToTargetTexture();
	void UpdateMultipleCameraOffsetAndBuffer();
	void ReleaseRenderTarget();
	
	bool IsPointInRenderArea( IntVec2 coords, IntVec2 textureSize, Polygon2 renderArea, AABB2 quickOutBox );
	AABB2 GetScreenRenderBox( IntVec2 size );
	AABB2 GetWorldSpaceRenderBox();
	Vec2 GetSplitScreenBoxDimension();
	Vec2 GetMultipleCameraOffset();

	void UpdateCameraPos();


	// helper function
	float GetMinimumInCircleRadiusForVoronoiHullWithPoint( Vec2 point );
private:
	// camera snapping
	float ComputeCameraSnapSpeed();
	Vec2 ComputeCameraWindowSnappedPosition( float deltaSeconds );
	void UpdateVoronoiPoly( PolyType type );
	void UpdateVoronoiHull();

public:
	static Texture* s_stencilTexture;

private:
	int m_index						= 0;
	bool m_isDebug					= false;
	Player* m_player				= nullptr; // const
	Camera* m_camera				= nullptr;
	Texture* m_stencilTexture		= nullptr;
	Texture* m_colorTarget			= nullptr;
	Shader* m_boxStencilShader		= nullptr;
	Shader* m_voronoiStencilShader	= nullptr;
	Shader* m_voronoiDiffuseShader	= nullptr;

	CameraSystem* m_owner		= nullptr;
	Timer*	m_timer				= nullptr;

	Vec2 m_playerPos						= Vec2::ZERO;
	Vec2 m_goalCameraPos					= Vec2::ZERO;
	Vec2 m_smoothedGoalCameraPos			= Vec2::ZERO;
	Vec2 m_cameraPos						= Vec2::ZERO;
	Vec2 m_cameraWindowCenterPos			= Vec2::ZERO;
	Vec2 m_multipleCameraRenderOffset		= Vec2::ZERO;
	Vec2 m_voronoiAnchorPointPos			= Vec2::ZERO;
	Vec2 m_originalVoronoiAnchorPointPos	= Vec2::ZERO;
	Vec2 m_targetVoronoiAnchorPointPos		= Vec2::ZERO;

	// Camera winodw
	AABB2 m_cameraWindow;
	
	// Camera snapping
	float m_snappingMinSpeed	= 2.f;
	float m_snappingMaxSpeed	= 4.f;
	float m_snappingSpeed		= 0.f;

	// camera smooth
	bool m_isSmooth				= true;
	float m_asymptoticValue		= 0.9f;
	float m_maxDeltaDist		= 6.f;
	float m_minAsymptotic		= 0.95f;
	float m_maxAsymptotic		= 0.995f;

	// Camera Shake
	float m_trauma				= 0.f;
	float m_traumaDecaySpeed	= 1.f;
	float m_shakeIntensity		= 0.f;
	float m_maxShakePosDist		= DEFAULT_MAX_POSITION_SHAKE_DISTANCE;
	float m_maxShakeRotDeg		= DEFAULT_MAX_ROTATION_SHAKE_DEGREE;

	// Camera framing
	bool m_useCameraFrame		= false;
	float m_fwdVelFocusDist		= DEFAULT_FORWARD_VELOCITY_FRAME_DIST;
	float m_aimFocusDist		= DEFAULT_AIM_FRAME_DIST;
	float m_fwdVelFocusRatio	= 0.f;
	float m_aimFocusRatio		= 0.f;
	float m_cueFocusRatio		= 0.f;

	// multiple player single camera
	bool m_ismultipleFactorStable					= true;
	float m_goalMultipleFactor						= 1.f;
	float m_startMultipleFactor						= 0.f;
	float m_currentMultipleCameraContributionsRatio	= 0.f;
	float m_contributionRatioStableSeconds			= 0.f; 


	// multiple camera setting
	RenderBuffer* m_offsetBuffer				= nullptr;
	RenderBuffer* m_voronoiOffsetBuffer			= nullptr;

	// voronoi setting
	bool			m_needRenderWhenMerged			= true;
	bool			m_allowMerge					= false;
	bool			m_allowBlend					= false;
	float			m_voronoiInCircleRadius			= 0.f;
	float			m_originalvoronoiInCircleRadius	= 0.f;
	float			m_voronoiPolyArea				= 0.f;
	float			m_originalVoronoiPolyArea		= 0.f;
	float			m_screenEdgeWidth				= 0.5f;
	float			m_maxedgeThickness				= 0.25f;
	Rgba8			m_splitScreenEdgeColor			= Rgba8::BLACK;
	Vec2			m_voronoiStencilOffset			= Vec2::ZERO;
	Vec2			m_voronoiColorTargetOffset		= Vec2::ZERO;
	Polygon2		m_voronoiPolygon;
	Polygon2		m_originalVoronoiPolygon;
	ConvexHull2		m_voronoiHull;
	std::vector<edgeAndThickness> m_blendingEdgeAndThickness;
	std::vector<Vertex_PCU> m_stencilVertices;

	// Debug
	Polygon2 m_debugVoronoiPoly;
};