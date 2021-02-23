#pragma once
#include <vector>
#include "Game/Camera/CameraController.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Math/ConvexHull2.hpp"

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
	AXIS_ALIGNED_SPLIT,
	VORONOI_SPLIT,
	NUM_OF_SPLIT_SCREEN_STATE,
};

enum NoSplitScreenStrat : unsigned int {
	NO_STRAT = 0,
	ZOOM_TO_FIT,
	KILL_AND_TELEPORT,
	NUM_OF_NO_SPLIT_SCREEN_STRAT
};

enum DebugMode: unsigned int {
	CONTROLLER_INFO = 0,
	HELP_MODE,
	NUM_OF_DEBUG_MODE
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
	void UpdatePostSplitScreenEffects( float deltaSeconds );
	void UpdateNoSplitScreenEffect( float deltaSeconds );
	void UpdateVoronoiSplitScreenEffect( float delteSeconds );
	void UpdateControllerCameras();
	void DebugRender();
	void DebugRenderControllers();

	// Accessor
	bool GetIsDebug() const { return m_isdebug; }
	bool DoesNeedSplitScreen( AABB2 cameraBox ) const;
	int GetControllersNum() const { return (int)m_controllers.size(); }
	float GetTotalFactor() const ; 

	std::string GetCameraWindowStateText() const;
	std::string GetCameraSnappingStateText() const;
	std::string GetCameraShakeStateText() const;
	std::string GetCameraFrameStateText() const;
	std::string GetSplitScreenStateText() const;
	std::string GetNoSplitScreenStratText() const;
	std::string GetDebugModeText() const;

	CameraWindowState	GetCameraWindowState() const { return m_cameraWindowState; }
	CameraSnappingState GetCameraSnappingState() const { return m_cameraSnappingState; }
	CameraShakeState	GetCameraShakeState() const { return m_cameraShakeState; }
	CameraFrameState	GetCameraFrameState() const { return m_cameraFrameState; }
	SplitScreenState	GetSplitScreenState() const { return m_splitScreenState; }
	NoSplitScreenStrat	GetNoSplitScreenStrat() const { return m_noSplitScreenStrat; }
	DebugMode			GetDebugMode() const { return m_debugMode; }

	Vec2 GetAverageCameraPosition();
	AABB2 GetWorldCameraBox();
	AABB2 GetSplitCheckBox();

	Camera*				GetNoSplitCamera() { return m_noSplitCamera; }
	CameraController*	GetCameraControllerWithPlayer( Player* player );

	std::vector<CameraController*>& GetCameraControllers();
	RandomNumberGenerator* GetRNG(){ return m_rng; }

	// Mutator
	void SetIsDebug( bool isDebug );
	void SetControllerSmooth( bool isSmooth );
	void SetCameraWindowState( CameraWindowState newState );
	void SetCameraSnappingState( CameraSnappingState newState );
	void SetCameraShakeState( CameraShakeState newState );
	void SetCameraFrameState( CameraFrameState newState );
	void SetSplitScreenState( SplitScreenState newState );
	void SetNoSplitScreenStrat( NoSplitScreenStrat newStrat ); 
	void SetDebugMode( DebugMode newMode );
	void SetMap( Map* map );

	void AddCameraShake( int index, float shakeTrauma );
	void AddTestingVoronoiOffsetX();
	void AddTestingVoronoiOffsetY();
	void DecreaseTestingVoronoiOffsetX();
	void DecreaseTestingVoronoiOffsetY();

	void UpdateDebugInfo();

	// Controller
	void CreateAndPushController( Player* player );
	void PrepareRemoveAndDestroyController( Player const* player );
	void UpdateControllerMultipleFactor( float smoothTime );
	int GetValidPlayerNum();

	// split screen strat
		// no split screen strat
	bool IsControllersInsideCamera();
	void GetNotInsideControllers( std::vector<CameraController*>& vec, float insidePaddingLength );
	bool CheckIfZoomStable( float stablePaddingLength );
	void KillAndTeleportPlayers( std::vector<CameraController*>& vec );
	void ZoomCameraToFitPlayers();
	float ComputeCameraHeight( std::vector<CameraController*> const& vec );

		// voronoi split
	void ConstructVoronoiDiagramForControllers( AABB2 worldCameraBox, AABB2 splitCheckBox );
	void ConstructVoronoiDiagramForTwoControllers( AABB2 worldCameraBox, AABB2 splitCheckBox );
	void ConstructVoronoiDiagramForThreeControllers( AABB2 worldCameraBox, AABB2 splitCheckBox );
	void ConstructVoronoiDiagramForMoreThanThreeControllers( AABB2 worldCameraBox, AABB2 splitCheckBox );
	void GetVoronoiHullsWithThreePointsInCollinearOrder( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC );
	void GetVoronoiHullsWithThreePointsNotCollinear( Vec2 a, Vec2 b, Vec2 c, AABB2 worldCameraBox, ConvexHull2& hullA, ConvexHull2& hullB, ConvexHull2& hullC );
	std::vector<Vec2> GetVoronoiPointsForCellWithTwoHelpPointsAndPBIntersectPoints( Vec2 point, Vec2 helpPointA, Vec2 helpPointB, AABB2 worldCameraBox, std::pair<Vec2, Vec2> PBAPoints, std::pair<Vec2, Vec2> PBBPoints );
	void GetNextVoronoiPolygonControllerWithIntersectPoint( std::pair<Vec2, Vec2> intersectPoints, CameraController* currentController, std::stack<CameraController*>& nextControllers );
	bool DoesNeedExpendVoronoiScreen();
	void ExpandMinVoronoiPoly( CameraController* minAreaController );

	// helper function
	CameraController* FindCurrentControllerContainsPointWithConstructedCellNum( Vec2 point, int constructedCellNum );
	CameraController* FindControllerWithMinArea();
	CameraController* FindNextAdjacentControllerWithPlane( Plane2 plane, std::vector<bool>& controllerCheckStates );
	bool GetSharedEdgeOfTwoPolygon( LineSegment2& sharedLine, Polygon2 polyA, Polygon2 polyB );
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
	DebugMode			m_debugMode				= CONTROLLER_INFO;
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

	Camera* m_noSplitCamera			= nullptr;
	Shader* m_multipleCameraShader	= nullptr;
	Map*	m_map					= nullptr;
	//Timer*

	// voronoi split
	float m_expandVoronoiAreaThreshold = 20.f;
	std::stack<CameraController*> m_nextControllers;
	std::vector<std::pair<CameraController*, std::vector<Plane2>>> m_controllerVoronoiEdges;
	// debug
	std::vector<Vec2> m_playerDebugPositions;
	Vec2 m_playerDebugPosA;
	Vec2 m_playerDebugPosB;
	Vec2 m_playerDebugPosC;
	Vec2 m_playerDebugPosX;
	Polygon2 m_DebugPolyA;
	Polygon2 m_DebugPolyB;
	Polygon2 m_DebugPolyC;
	Polygon2 m_DebugPolyD;
	ConvexHull2 m_debugHullA;
	ConvexHull2 m_debugHullB;
	ConvexHull2 m_debugHullC;

	std::vector<Vec2> m_debugIntersectPointsA;
	std::vector<Vec2> m_debugIntersectPointsB;
	std::vector<Vec2> m_debugIntersectPointsC;
	std::vector<Vec2> m_debugIntersectPointsX;
	std::vector<std::vector<Vec2>> m_debugAfterDeletePoints;
	std::vector<Vec2> m_debugLineIntersectWithBoxPointsAB;
	std::vector<Vec2> m_debugLineIntersectWithBoxPointsAC;
	std::vector<Vec2> m_debugLineIntersectWithBoxPointsBC;
	std::vector<LineSegment2> m_debugPBLines;	

};