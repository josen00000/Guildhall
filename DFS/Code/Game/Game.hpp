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
class GameObject;
class World;
class UIButton;

struct Vertex_PCU;

enum GameState
{
	ATTRACT_SCREEN,
	IN_GAME,
	GAME_END
};


class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	//basic
	void Startup();
	void RestartGame();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void RenderGame() const;
	void RenderUI() const;
	void SetGameEnd();

	// Accessor
	bool GetIsDebug() const { return m_isDebug; }

	// mutator
	void SetIsDebug( bool isDebug );
	// game object
	void DeleteGameObject( GameObject* obj );
	void CleanDestroyedObjects();

private:
	void UpdateGame( float deltaSeconds );
	void UpdateUI( float deltaSeconds );
	void RenderGameUI() const;

	//Game State Check
	void CheckIfExit();

	// GetInput
	void HandleKeyboardInput();
	void HandleAttractInput();

	//Load
	void LoadAssets();
	void LoadDefs();

	// attractScreen
	void UpdateAttractScreen();
	void RenderAttractScreen() const;
	void SelectPreviousButton();
	void SelectNextButton();
	void CreateAttractButtons();

	bool StartGame( EventArgs& args );
	bool QuitGame( EventArgs& args );
	bool LoadSettings( EventArgs& args );
	bool SetVolumeHigh( EventArgs& args );
	bool SetVolumeLow( EventArgs& args );
	bool SetVolumeMedium( EventArgs& args );
	bool BackToMain( EventArgs& args );
	bool DebugGameInfo( EventArgs& args );

public:
	bool m_isAppQuit		= false;
	bool m_isDebug			= false;
	bool m_isFighting		= false;
	bool m_isOnSettingPage	= false;

	GameState m_gameState	= ATTRACT_SCREEN;
	std::vector<UIButton*> m_buttons;
	std::vector<UIButton*> m_settingButtons;
	int m_currentButtonIndex = 0;
	int m_currentSettingButtonIndex = 0;
	

	World* m_world			= nullptr;

	RandomNumberGenerator* m_rng = nullptr;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;


	std::vector<GameObject*> m_gameObjects;
	float m_playerHP				= 0.f;
	float m_enemyHP					= 0.f;
	float m_playerAttackStrength	= 0.f;
	float m_enemyAttackStrength		= 0.f;
	std::string m_enemyType			= "";

	// audio
	float m_volume					= 0.5f;
	SoundPlaybackID m_gameBGMID		= 0;
};
