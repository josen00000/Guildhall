#pragma once
#include<Engine/Math/vec2.hpp>
#include<Game/GameCommon.hpp>
#include<Engine/Math/RandomNumberGenerator.hpp>
#include<Engine/Input/KeyBoardController.hpp>
class RenderContext;
class InputSystem;
class Camera;
class Map;
class Player;
class Turn;
class Entity;
enum GameStage {
	ERROR_STAGE=-1,
	TURN_START,
	PLAYER_MOVE,
	PLAYER_SPREAD,
	TURN_END,
	GAME_END,
	GAME_STAGE_NUM
};
class Game {
public:
	Game(){}
	~Game(){}
	Game(RenderContext* gameRenderer,InputSystem* inputSystem, bool isDevelop,Camera* inCamera);
	//basic
	void Startup();
	void Shutdown();
	void RunFrame(float deltaTime);
	const void Render();
	void ControlCollision();
	void RenderUI();
	void Reset();
	void UpdateCamera(float deltaTime);
	void UpdateMap();
	void CheckSuccess();
	//Player
	void MovePlayer(Player* inPlayer);
private:
	void Update(float deltaTIme);
	void InitialPlayer(Player* inPlayer);

public:

	bool m_developMode=false;
	bool isAppQuit=false;
	bool m_isAttractMode=false;
	bool m_isFirstPlayer=true;
	GameStage m_gameStage=TURN_START;
	Player* m_player1=nullptr;
	Player* m_player2=nullptr;
	Turn* m_turn=nullptr;
	RenderContext* m_theRenderer=nullptr;
	InputSystem* m_theInput=nullptr;
	Camera* m_camera=nullptr;
	Map* m_map=nullptr;
	RandomNumberGenerator m_rng;
	float m_screenShakeIntensity=0;
	float m_screenShakeDecreaseRate=0;



};