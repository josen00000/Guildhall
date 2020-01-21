#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"

class Map;
class Texture;

enum VictorySceneState {
	VICTORY_SCENE_NULL,
	VICTORY_SCENE_FADE_IN,
	VICTORY_SCENE_WAIT_FOR_READY_INPUT,
	VICTORY_SCENE_WAIT_FOR_INPUT,
	NUM_VICTORY_SCENE_STATE
};

class World
{
public:
	World();
	~World(){}
	void Update(float deltaSeconds);	
	void Render()const ;
	void DestroyEntities();
	void EnterLoseScene();
	void EnterWinScene();
	void RenderUI()const ;

	//victory scene
	void InitializeVictoryScene();
	void UpdatePlayerVictoryScene( float deltaSeconds );
	void RenderPlayerVictoryScene() const;
public:
	Map* m_currentMap=nullptr;
	Map* m_maps[4]={};
	int m_mapIndex = 0;
	int m_mapCreateIndex = 0;
	int m_playerLife = 3;
	Texture* m_texture = nullptr;
	Texture* m_texture1 = nullptr;
	bool m_isRenderGame = true;
	bool m_isWin = false;
	bool m_isVictorySceneInitialized = false; // not need. need to modify the struct.
	float m_victorySceneFadeInTime = 0.f;
	float m_victorySceneWaitInputTime = 0.f;
	VictorySceneState m_victorySceneState = VICTORY_SCENE_NULL;
	AABB2 m_victoryScene;
	Rgba8 m_victorySceneColor;
};

