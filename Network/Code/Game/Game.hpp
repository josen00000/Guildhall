#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/DevConsole.hpp"

class Camera;
class World;
class GameObject;
class CubeSphere;
class GPUMesh;
class InputSystem;
class RenderContext;
class RandomNumberGenerator;
class Texture;
class ShaderState;
class Material;
class Shader;
class RenderBuffer;
class TCPClient;
class TCPServer;

struct Vertex_PCU;
struct Mat44;

extern DevConsole*		g_theConsole;

enum GameState {
	GAME_STATE_NULL = -1,
	GAME_STATE_LOADING,
	GAME_STATE_ATTRACT,
	GAME_STATE_PLAYING,
	GAME_STATE_END,
	NUM_GAME_STATE
};

struct ColorData {
	Mat44 colorMat;
	float pow;
	Vec3 pad;
};

class Game {
public:
	Game(){}
	~Game(){}
	explicit Game( Camera* gameCamera, Camera* UICamera );

	//basic
	void Startup();
	void Shutdown();
	void Update( float deltaSeconds );
	void Reset();
	void RenderGame() const;
	void RenderUI() const;
	void RenderBasis() const;
	
	// UI
	void RenderBaseHud() const;
	void RenderGun() const;


	// map
	bool LoadMapWithName( std::string mapName );
	Strings GetAllMaps() const;

	// convension
	const Convention GetConvention() const{ return m_convension; }
	void SetConvention( Convention convention ); 

	void SetCameraPos( Vec3 pos );
	void SetCameraYaw( float yaw );


	Entity* GetPlayer() { return m_player; }

	Texture* GetMapTexture();
	std::vector<Vertex_PCU>& GetMapRenderData();
	SpriteSheet* GetViewModelSpriteSheet();

private:
	void UpdateUI( float deltaSeconds );
	void UpdateCamera(float deltaSeconds );
	void UpdateLighting( float deltaSeconds );

	// billboard test
	void UpdateBillboardTest();
	void RenderBillboardTest() const;

	// raycast test
	void RaycastTest( Vec3 startPos, Vec3 forwardNormal, float maxDist );

	// jobsystem test
	void JobTest();


	void UpdatePlayer( float delteSeconds );

	// render
	void RenderDebugInfo() const;
	void RenderFPSInfo() const;

	// input
	void HandleInput( float deltaSeconds );
	void HandleKeyboardInput( float deltaSeconds );
	void HandleCameraMovement( float deltaSeconds );
	void HandleAudioKeyboardInput();
	void PlayerTestSound();

	//Game State Check
	void CheckIfExit();
	void CheckGameStates();
	void CheckIfPause();
	void CheckIfDeveloped();
	void CheckIfGhost();

	//Load
	void LoadAssets();
	void LoadDefinitions();
	void LoadMapsDefinitionsAndCreateMaps( const char* mapFolderPath );
	template<typename T>
	void LoadFileDefinition( T t, const char* path );

	// Mesh
	void CreateGameObjects();

	// player
	void PossessEntityAsPlayer();


public:
	bool m_debugMode		= false;
	bool m_ghostMode		= false;
	bool m_isPause			= false;
	bool m_debugCamera		= false;
	bool m_isAppQuit		= false;
	bool m_isAttractMode	= false;
	bool m_isUsingGrayEffect = false;
	bool m_isUsingBloomEffect = false;

	GameState m_gameState	= GAME_STATE_NULL;
	Camera* m_gameCamera	= nullptr;
	Camera* m_UICamera		= nullptr;
	RandomNumberGenerator* m_rng = nullptr;
	
	GPUMesh* m_cubeMesh	= nullptr;
	GameObject* m_cubeObj = nullptr;
	GameObject* m_cubeObj1 = nullptr;
	GameObject* m_cubeObj2 = nullptr;

	// billboard Test
	Entity* m_billBoardEntity = nullptr;
	Entity* m_player		= nullptr;
	BillboardMode m_billboardMode = BILLBOARD_MODE_CAMERA_FACING_XY;
	
	std::vector<Vertex_PCU> m_vertices;
	std::vector<Vertex_PCU> m_UIVertices;
	// convension
	Convention m_convension;

	World* m_world = nullptr;

	SpriteSheet* m_viewModelSpriteSheet;

private:
	float m_DebugDeltaSeconds = 0.f;
};


template<typename T>
void Game::LoadFileDefinition( T t, const char* path )
{
	XmlDocument defFile;
	if( defFile.LoadFile( path ) != 0 ) {
		ERROR_AND_DIE( Stringf(  "Cannot Open xml file with path:%s.", path ));
		//g_theConsole->DebugErrorf( "Cannot Open xml file with path:%s.", path );
		return;
	}
	const XmlElement* mapRootElement = defFile.RootElement();
	t( *mapRootElement );
}

// Map Command
bool MapCommandLoadMap( EventArgs& args );
bool MapCommandWarp( EventArgs& args );
bool SendMessageTest( EventArgs& args );
bool StartServer( EventArgs& args );
bool SendData( EventArgs& args );
bool StopServer( EventArgs& args );
bool Disconnect( EventArgs& args );
bool OpenUDPPort( EventArgs& args );
bool SendUDPMessage( EventArgs& args );
bool CloseUDPPort( EventArgs& args );