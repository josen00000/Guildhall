#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Game/GameObject.hpp"

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
class SpriteSheet;
class SpriteAnimDefinition;

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
	void RenderGame() const;
	void RenderUI() const;
	void EndFrame();

	// Accessor
	bool GetIsDebug() const { return m_isDebug; }
	Map* GetCurrentMap();
	Texture* GetAimTexture() { return m_aimTexture; }

	// mutator
	void SetIsDebug( bool isDebug );
	void SetIsPaused( bool isPaused );

private:
	void HandleInput();
	void UpdateGame( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void RenderGameUI() const;

	//Game State Check
	void CheckIfExit();
	void CheckIfDebug();

	//Load
	void LoadAssets();
	void LoadDefs();

public:
	bool m_isAppQuit		= false;
	bool m_isPaused			= false;
	bool m_isDebug			= false;
	bool m_isFighting		= false;
	bool m_isOnSettingPage	= false;
	bool m_showPauseMenu	= false;

	int m_currentButtonIndex = 0;
	int m_currentSettingButtonIndex = 0;
	
	World* m_world			= nullptr;
	Texture* m_enemyTexture					= nullptr;
	Texture* m_enemyBarrelTexture			= nullptr;
	Texture* m_playerTexture				= nullptr;
	Texture* m_playerBarrelTexture			= nullptr;
	Texture* m_playerProjectileTexture		= nullptr;
	Texture* m_enemyProjectileTexture		= nullptr;
	Texture* m_aimTexture					= nullptr;
	SpriteSheet* m_explodeSpriteSheet		= nullptr;
	SpriteAnimDefinition* m_explodeAnimDef	= nullptr;
	CameraSystem* m_cameraSystem = nullptr;
	Camera* m_gameCamera		= nullptr;
	Camera* m_UICamera			= nullptr;

	std::vector<Coroutine*> m_gameObjects;
};

bool SetCameraAsymptoticValue( EventArgs& args );
bool SpawnItem( EventArgs& args );
bool DeletePlayer( EventArgs& args );
bool SetIsSmooth( EventArgs& args );