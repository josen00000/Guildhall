#pragma once
#include <vector>
#include "../../Thesis/Code/Game/Camera/CameraController.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Math/ConvexHull2.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Camera.hpp"

typedef	std::pair<Vec2, std::vector<CameraController*>> VoronoiVertexAndControllersPair;
//typedef std::pair<CameraController*, float> ControllerVoronoiSplitBlendCoeff;

class Player;
class Camera;
class RandomNumberGenerator;

enum eDebugBitFlag : uint {
	DEBUG_NONE						= 0,
	DEBUG_CAMERA_WINDOW_BIT			=	BIT_FLAG(0),
	DEBUG_CAMERA_POSITION_BIT		=	BIT_FLAG(1),
	DEBUG_VORONOI_INCIRCLE_BIT		=	BIT_FLAG(2),	
	DEBUG_VORONOI_ANCHOR_POINT_BIT	=	BIT_FLAG(3),
};

enum CameraWindowState: unsigned int {
	NO_CAMERA_WINDOW = 0,
	USE_CAMERA_WINDOW,
	NUM_OF_CAMERA_WINDOW_STATE,
};

enum CameraSnappingState : unsigned int {
	NO_CAMERA_SNAPPING = 0,
	POSITION_SNAPPING,
	POSITION_HORIZONTAL_SNAPPING,
	POSITION_VERTICAL_SNAPPING,
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
	FORWARD_VELOCITY_FRAMING,
	AIM_FRAMING,
	CUE_FRAMING,
	BLEND_FRAMING,
	NUM_OF_FRAME_STATE
};

enum SplitScreenState : unsigned int {
	NO_SPLIT_SCREEN = 0,
	AXIS_ALIGNED_SPLIT,
	VORONOI_SPLIT,
	NUM_OF_SPLIT_SCREEN_STATE,
};

enum NoSplitScreenStrat : unsigned int {
	NO_STRAT = 0,
	ZOOM_TO_FIT,
	KILL_AND_TELEPORT,
	COMBINATION_ZOOM_AND_KILL,
	NUM_OF_NO_SPLIT_SCREEN_STRAT
};

enum DebugMode: unsigned int {
	CONTROLLER_INFO = 0,
	VORONOI_INFO,
	HELP_MODE,
	NUM_OF_DEBUG_MODE
};

enum PostVoronoiSetting : unsigned int{
	NO_POST_EFFECT = 0,
	PUSH_PLANE_AND_REARRANGE_VORONOI_VERTEX,
	BALANCED_VORONOI_DIAGRAM,
	HEALTH_BASED_VORONOI_DIAGRAM,
	NUM_OF_POST_VORONOI_SETTING
};

enum VoronoiAreaCheckState : unsigned int {
	POLYGON_AREA,
	INCIRCLE_RADIUS,
	NUM_OF_AREA_CHECK_STATE
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
	void Render();
	void UpdateControllers( float deltaSeconds );
	void UpdateSplitScreenEffects( float deltaSeconds );
	void UpdateNoSplitScreenEffect( float deltaSeconds );
	void UpdatePreVoronoiSplitScreenEffect();	// different split dicision, confirm the voronoi cells for cameraController
	void UpdateVoronoiSplitScreenEffect();		// Construct the convexhull for each camera
	void UpdatePostVoronoiSplitScreenEffects();	// Get voronoi poly base on convexhull and do area average.
	void UpdateControllerCameras();
	void UpdateDebugInfo();
	void DebugRender();
	void DebugRenderControllers();

	// Accessor
	bool GetIsDebug() const { return m_isdebug; }
	bool GetAllowMergeVoronoi()	const{ return m_doesAllowVoronoiMerge; }
	int GetControllersNum() const { return (int)m_controllers.size(); }
	int GetPostVoronoiIterationNum() const { return m_postVoronoiIteration; }
	float GetTotalFactor() const ; 
	float GetForwardVelocityCameraFrameDist() const;
	float GetAimCameraFrameDist() const;
	float GetControllerForwardVelocityCameraFrameRatio( int controllerIndex ) const;
	float GetControllerAimCameraFrameRatio( int controllerIndex ) const;
	float GetControllerCueCameraFrameRatio( int controllerIndex ) const;
	float GetPositionalShakeMaxDist() const;
	float GetRotationalShakeMaxDeg() const;
	Vec2 GetCameraWindowSize() const;
	Vec2 GetSystemCameraPosition() const { return m_noSplitCamera->GetPosition2D(); }

	// Debug flags
	bool GetDoesDebugCameraWindow() const;
	bool GetDoesDebugAnchorPoints() const;
	bool GetDoesDebugCameraPos() const;

	std::string GetCameraWindowStateText() const;
	std::string GetCameraSnappingStateText() const;
	std::string GetCameraShakeStateText() const;
	std::string GetCameraFrameStateText() const;
	std::string GetSplitScreenStateText() const;
	std::string GetNoSplitScreenStratText() const;
	std::string GetDebugModeText() const;
	std::string GetPostVoronoiSettingText() const;
	std::string GetVoronoiAreaCheckStateText() const;

	//const char** GetAllCameraWindowText() const;

	CameraWindowState		GetCameraWindowState() const { return m_cameraWindowState; }
	CameraSnappingState		GetCameraSnappingState() const { return m_cameraSnappingState; }
	CameraShakeState		GetCameraShakeState() const { return m_cameraShakeState; }
	CameraFrameState		GetCameraFrameState() const { return m_cameraFrameState; }
	SplitScreenState		GetSplitScreenState() const { return m_splitScreenState; }
	NoSplitScreenStrat		GetNoSplitScreenStrat() const { return m_noSplitScreenStrat; }
	PostVoronoiSetting		GetPostVoronoiSetting() const { return m_postVoronoiSetting; }
	VoronoiAreaCheckState	GetVoronoiAreaCheckState() const { return m_voronoiAreaCheckState; }
	DebugMode			GetDebugMode() const { return m_debugMode; }

	Vec2 GetAverageCameraPosition();
	AABB2 GetWorldCameraBox();
	AABB2 GetVoronoiBox();

	Camera*				GetNoSplitCamera() { return m_noSplitCamera; }
	CameraController*	GetCameraControllerWithPlayer( Player* player );

	std::vector<CameraController*>& GetCameraControllers();
	CameraController*	GetCameraControllerWithIndex( int index );
	RandomNumberGenerator* GetRNG(){ return m_rng; }

	// Mutator
	void SetIsDebug( bool isDebug );
	void SetControllerSmooth( bool isSmooth );
	void SetAllowMerge( bool allowMerge );
	void SetCameraWindowState( CameraWindowState newState );
	void SetCameraSnappingState( CameraSnappingState newState );
	void SetCameraShakeState( CameraShakeState newState );
	void SetCameraFrameState( CameraFrameState newState );
	void SetSplitScreenState( SplitScreenState newState );
	void SetNoSplitScreenStrat( NoSplitScreenStrat newStrat ); 
	void SetDebugMode( DebugMode newMode );
	void SetPostVoronoiSetting( PostVoronoiSetting postSetting );
	void SetVoronoiAreaCheckState( VoronoiAreaCheckState voronoiAreaCheckState );
	void SetMap( Map* map );
	void SetCameraWindowSize( Vec2 dimension );

	void AddCameraShake( int index, float shakeTrauma );
	
	void SetForwardVelocityFrameDistance( float dist );
	void SetAimFocusDistance( float dist );
	void SetControllerAimFocusDistance( int controllerindex, float dist );
	void SetControllerForwardVelocityFrameRatio( int controllerIndex, float ratio );
	void SetControllerAimFocusRatio( int controllerIndex, float ratio );
	void SetControllerCueFocusRatio( int controllerIndex, float ratio );
	void SetPositionalShakeMaxDist( float maxDist );
	void SetRotationalShakeMaxDeg( float maxDeg );
	void SetPostVoronoiIterationNum( int num );

	// Debugflags
	void SetDoesDebugCameraWindow( bool doesDebugCameraWindow );
	void SetDoesDebugCameraPosition( bool doesDebugCameraPosition );
	void SetDoesDebugAnchorPoints( bool doesDebugAnchorPoints );

	// Controller
	void CreateAndPushController( Player* player );
	void DestroyAllControllers();
	void PrepareRemoveAndDestroyController( Player const* player );
	void UpdateControllerMultipleFactor( float smoothTime );
	void CheckIsControllerVaild( int controllerIndex ) const;
	int GetValidPlayerNum();

	// split screen strat
		// no split screen strat
	void GetNotInsideControllers( std::vector<CameraController*>& vec, float insidePaddingLength );
	bool CheckIfZoomStable( float stablePaddingLength );
	void KillAndTeleportPlayers( std::vector<CameraController*>& vec );
	void ZoomCameraToFitPlayers();
	float ComputeCameraHeight( std::vector<CameraController*> const& vec );

		// Axis Aligned split screen
	void SetDynamicAxisAlignedSplitScreenMultipleFactors( float smoothTime, float factor1, float factor2 );
		// voronoi split
	void ConstructVoronoiDiagramForControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type );
	void ConstructVoronoiDiagramForTwoControllers( AABB2 worldCameraBox, AABB2 voronoiBox );
	void ConstructVoronoiDiagramForThreeControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type );
	void ConstructVoronoiDiagramForMoreThanThreeControllers( AABB2 worldCameraBox, AABB2 voronoiBox, PolyType type );
	void UpdateVoronoiPolyAndOffset();
	void ConstructVoronoiHullsWithThreePointsInCollinearOrder( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC );
	void ConstructVoronoiHullsWithThreePointsNotCollinear( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC );
	
	// post voronoi
	bool DoesNeedBalanceVoronoiScreen();
	void ExpandMinVoronoiPoly();
	bool ConstructAllAndIsAnyVoronoiVertex();
	bool RearrangeVoronoiVertexForMinVoronoiPoly( CameraController* controller, std::vector<Vec2> voronoiVertexPos );
	void ComputeAndUpdateOriginalVoronoiAnchorPoints( AABB2 worldCameraBox, AABB2 voronoiBox, bool isInitialized  );
	void ComputeAndSetBalancedVoronoiTargetAnchorPoints( AABB2 worldCameraBox, AABB2 voronoiBox );
	void InitializeVoronoiTargetPointWithVoronoiOriginalPoint();
	void ReconstructVoronoiDiagramWithTargetAnchorPoints();
	float GetAverageVoronoiInCircleRadius();
	float GetAverageOriginalVoronoiInCircleRadius();
	float GetMinVoronoiInCircleRadius();
	float GetMinOriginalVoronoiInCircleRadius();

	// split and merge
	bool IsControllerTotalSplit( int controllerIndex );
	bool IsControllerMerged( int controllerIndex );
	bool IsControllerNeedRenderWhenMerged( int controllerIndex );

	Vec2 GetTotalSplitColorTargetOffset( int controllerIndex );
	Vec2 GetSplitToMergeBlendingColorTargetOffsetAndBlendingEdges( int controllerIndex, std::vector<edgeAndThickness>& blendingEdgeIndexes );
	Vec2 GetBlendingColorTargetOffsetAndBlendingEdgeIndexesOfTwoControllers( int controllerIndexA, int controllerIndexB,  std::vector<edgeAndThickness>& edgeIndexes  );
	Vec2 GetMergedColorTargetOffset( std::vector<int> controllerIndexes );
	void GetTotalMergedColorTargetOffsetAndMergedPoly( int controllerIndexA, Vec2& mergedColorTargetOffset, Polygon2& mergedPoly );
	void GetMergedVoronoiPolygonWithTwoControllers( CameraController* a, CameraController* b, Polygon2& mergedPoly );
	LineSegment2 GetBlendingEdgeAndMergedVoronoiPolygonWithTwoControllers( CameraController* a, CameraController* b, Polygon2& mergedPoly );
	void GetMergedVoronoiPolygonWithThreeControllers( CameraController* a, CameraController* b, CameraController* c,  Polygon2& mergedPoly );


	// helper function
	bool GetSharedPointOfVoronoiPolygonABC( Polygon2 polygonA, Polygon2 polygonB, Polygon2 polygonC, Vec2& sharedPoint );
	bool GetAllVoronoiVertexPosWithController( const CameraController* controller, std::vector<Vec2>& vertexPosArray );
	CameraController* FindCurrentControllerContainsPointWithConstructedCellNum( Vec2 point, int constructedCellNum );
	CameraController* FindControllerWithMinArea();
	CameraController* FindControllerWithMinRadius();
	CameraController* FindNextAdjacentControllerWithPlane( Plane2 plane, std::vector<bool>& controllerCheckStates );
	void GetMergedPolygonWithTwoPolygonShareOneEdge( const Polygon2& polyA, const Polygon2& polyB, Polygon2& mergedPoly );

private:
	bool m_isdebug					= false;
	uint m_debugFlags				= 0;
	std::vector<CameraController*> m_controllers;
	
	// states
	CameraWindowState		m_cameraWindowState		= NO_CAMERA_WINDOW;
	CameraSnappingState		m_cameraSnappingState	= NO_CAMERA_SNAPPING;
	CameraShakeState		m_cameraShakeState		= POSITION_SHAKE;
	CameraFrameState		m_cameraFrameState		= NO_FRAMEING;
	SplitScreenState		m_splitScreenState		= NO_SPLIT_SCREEN; // Temp for demo TODO: change back
	NoSplitScreenStrat		m_noSplitScreenStrat	= NO_STRAT;
	DebugMode				m_debugMode				= CONTROLLER_INFO;
	PostVoronoiSetting		m_postVoronoiSetting	= NO_POST_EFFECT;
	VoronoiAreaCheckState	m_voronoiAreaCheckState	= POLYGON_AREA;
	RandomNumberGenerator* m_rng	= nullptr;


	// no split settings
	float m_notInsidePaddingLength			= 3.f;
	float m_zoomStablePaddingLength			= 4.f;
	float m_idealCameraHeight				= GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y;
	float m_maxCameraHeight					= ( GAME_CAMERA_MAX_Y - GAME_CAMERA_MIN_Y ) * 1.5f;
	float m_maxCameraDeltaHeightPerFrame	= 0.01f;

	Vec2  m_goalCameraPos		= Vec2::ZERO;

	Camera* m_noSplitCamera			= nullptr;
	Shader* m_multipleCameraShader	= nullptr;
	Map*	m_map					= nullptr;

	// voronoi split
	std::vector<ConvexHull2> m_controllerVoronoiEdges;
	std::vector<VoronoiVertexAndControllersPair> m_voronoiVertices;
	float m_controllersSplitToMergeBlendCoeffs[MAX_CAMERA_CONTROLLER_NUM][MAX_CAMERA_CONTROLLER_NUM];
	
	// post voronoi setting
	bool m_isVoronoiInitialized					= false;
	bool m_doesAllowVoronoiMerge				= false;
	int m_postVoronoiIteration								= 0;
	int m_targetPostVoronoiIteration						= 3;
	float m_expandVoronoiLeastStep							= 2.f;

	// voronoi fairness
	float m_maxVoronoiAnchorPointMoveDistWithPolyArea		= 1.f;	
	float m_maxVoronoiAnchorPointMoveDistWithIncircleRadius	= 3.f;	
	float m_expandVoronoiAreaThreshold						= 50.f;
	float m_voronoiInCircleRadiusThreshold					= 5.f;

	// voronoi split & merge
	float m_totalSplitDist									= 15.f;   // 20
	float m_totalMergedDist									= 10.f;   // 15
	
	// debug
	Vec2 m_debugMergedPoint									= Vec2::ZERO;
};