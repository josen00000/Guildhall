#pragma once
#include <vector>
#include <string>
#include "Game/GameCommon.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

class RenderContext;
class InputSystem;
class Camera;
class Entity;
class World;
class BitmapFont;
class DevConsole;


class Game {
public:
	Game(){}
	~Game(){}
	Game(Camera* inCamera);

	//basic
	void Startup();
	void Shutdown();
	void RunFrame(float deltaTime);
	void Render() const;
	void RenderUI() const;
	
private:
	void Update( float deltaTime);
	void TestSprite();
	void TestImage();
	void TestDrawMouse( const Camera& camera);
	void TestSetFromText();
	void TestMouse();
	void RenderMouse( const Camera& camera) const;
	void TestSplitString();
	void InitialAlignText();
	void UpdateAlignText(float deltaTime);
	void UpdateTestString();
	void LoadGameAsset();


public:
	bool m_debugCamera = false;
	bool isAppQuit = false;
	//should using global.
	
	//mouse testing
	Vec2 m_mousePos;
	Camera* m_camera = nullptr;
	RandomNumberGenerator m_rng;
	std::string m_alignTestString;
	AABB2 m_alignBox = AABB2(Vec2(100,50), Vec2(120,70));
	AABB2 m_splitBox = AABB2(Vec2(80,50), Vec2(120,90));
	std::vector<Vertex_PCU> m_alignTextVertices;
	std::vector<Vertex_PCU> m_splitStringVertices;
	Vec2 m_alignPos = Vec2(0,1);
	//std::vector<Strings> m_stringsVector;
	float m_alignMoveSpeed = 0.5;
	BitmapFont* m_testFont = nullptr;
	DevConsole* m_testConsole = nullptr;
};
