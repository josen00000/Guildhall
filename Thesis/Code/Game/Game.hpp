#pragma once
#include <vector>
#include "Game/GameCommon.hpp"

#include "Engine/Math/vec2.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/Polygon2.hpp"

class RenderContext;
class InputSystem;
class RandomNumberGenerator;
class Camera;
class World;
class CameraSystem;
class Map;
class Texture;

struct Vertex_PCU;

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	// Game flow
	void Startup();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void RenderGame( int controllerIndex = 0 ) const;
	void RenderUI() const;
	void EndFrame();

	// Accessor
	bool GetIsDebug() const { return m_isDebug; }
	Map* GetCurrentMap();

	// mutator
	void SetIsDebug( bool isDebug );
	void SetIsPaused( bool isPaused );
	void InitializeCameraSystemSetting( int mapIndex );
	void InitializeMapSetting( int mapIndex );
	bool DisableCentralize( EventArgs& args );

private:
	void HandleInput();
	void UpdateGame( float deltaSeconds );
	void UpdateFrameRate( float deltaSeconds );
	void UpdateUI( );
	void UpdateSystemUI();
	void UpdateCameraWindowUI();
	void UpdateCameraFrameUI();
	void UpdateCameraShakeUI();
	void UpdateMultiplayerStrategyUI();
	void UpdateSystemDebugModeUI();
	void RenderGameUI() const;
	void PrepareDemo1();

	//Game State Check
	void CheckIfExit();

	//Load
	void LoadAssets();
	void LoadDefs();

public:
	bool m_isAppQuit		= false;
	bool m_isPaused			= false;
	bool m_isDebug			= false;
	bool m_isOnSettingPage	= false;
	bool m_useSystemUI		= false;
	bool m_useUI			= true;
	float m_maxForwardDist  = 5.f;
	float m_maxAimDist		= 2.5f;
	
	World* m_world			= nullptr;
	Texture* m_enemyTexture				= nullptr;
	Texture* m_enemyBarrelTexture		= nullptr;
	Texture* m_playerTexture			= nullptr;
	Texture* m_playerBarrelTexture		= nullptr;
	Texture* m_playerProjectileTexture	= nullptr;
	Texture* m_enemyProjectileTexture	= nullptr;
	Texture* m_bossTexture				= nullptr;
	Texture* m_bossBarrelTexture		= nullptr;


	CameraSystem* m_cameraSystem = nullptr;
	Camera* m_gameCamera		= nullptr;
	Camera* m_UICamera			= nullptr;

};

bool SetCameraAsymptoticValue( EventArgs& args );
bool SpawnItem( EventArgs& args );
bool DeletePlayer( EventArgs& args );
bool SetIsSmooth( EventArgs& args );
bool LoadMap( EventArgs& args );