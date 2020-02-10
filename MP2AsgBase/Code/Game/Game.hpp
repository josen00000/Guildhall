#pragma once
#include <vector>
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"

class RenderContext;
class InputSystem;
class RandomNumberGenerator;
class Camera;
class GameObject;
class World;

struct Vertex_PCU;

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	//basic
	void Startup();
	void Shutdown();
	void RunFrame( float deltaSeclnds );
	void Render() const;
	void RenderUI() const;

	
private:
	void Update( float deltaSeconds );
	void UpdateUI( float deltaSeconds );

	//Game State Check
	void CheckIfExit();

	//Load
	void LoadAssets();


public:
	bool m_isAppQuit		= false;

	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
	RandomNumberGenerator* m_rng = nullptr;
};
