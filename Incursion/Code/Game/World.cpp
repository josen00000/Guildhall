#include "World.hpp"
#include "Game/Map.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"

extern RenderContext* g_theRenderer;
extern Camera* g_UICamera;
extern InputSystem* g_theInputSystem;
extern Game* g_theGame;
extern BitmapFont* g_squirrelFont;

World::World()
{
	//map 1 setting
	int map1NumWorm[] = {20, 5};
	int map1WormLength[] = {5, 5};
	//map 2 setting
	int map2NumWorm[] ={ 10,10 };
	int map2WormLength[] ={ 5,5 };
	//map 3 setting
	int map3NumWorm[] ={ 10,10 };
	int map3WormLength[] ={ 8,5 };
	//map 4 setting
	int map4NumWorm[] ={ 10,10,10 };
	int map4WormLength[] ={ 5,7,6 };

	//map 1 setting
	TileType map1DefaultType = TileType::TILE_TYPE_GRASS;
	TileType map1EdgeType = TileType::TILE_TYPE_STONE;
	//map 2 setting
	TileType map2DefaultType = TileType::TILE_TYPE_GRASS;
	TileType map2EdgeType = TileType::TILE_TYPE_STONE;
	//map 3 setting
	TileType map3DefaultType = TileType::TILE_TYPE_GRASS;
	TileType map3EdgeType = TileType::TILE_TYPE_STONE;
	//map 4 setting
	TileType map4DefaultType = TileType::TILE_TYPE_GRASS;
	TileType map4EdgeType = TileType::TILE_TYPE_STONE;
	
	//map 1 setting
	std::vector<TileType> map1WormType = {TileType::TILE_TYPE_STONE, TileType::TILE_TYPE_BRICK};
	//map 2 setting
	std::vector<TileType> map2WormType ={ TileType::TILE_TYPE_STONE, TileType::TILE_TYPE_MUD };
	//map 3 setting
	std::vector<TileType> map3WormType ={ TileType::TILE_TYPE_STONE, TileType::TILE_TYPE_WATER };
	//map 4 setting
	std::vector<TileType> map4WormType ={ TileType::TILE_TYPE_STONE, TileType::TILE_TYPE_LAVA	, TileType::TILE_TYPE_WATER};


	m_maps[0] = new Map( this, IntVec2( MAP_SIZE_X, MAP_SIZE_Y ),map1WormType, map1NumWorm, map1WormLength, map1EdgeType, map1DefaultType );
	m_mapCreateIndex++;
	m_maps[1] = new Map( this, IntVec2( MAP_SIZE_X, MAP_SIZE_Y ),map2WormType, map2NumWorm, map2WormLength, map2EdgeType, map2DefaultType );
	m_mapCreateIndex++;
	m_maps[2] = new Map( this, IntVec2( MAP_SIZE_X, MAP_SIZE_Y ),map3WormType, map3NumWorm, map3WormLength, map3EdgeType, map3DefaultType );
	m_mapCreateIndex++;
	m_maps[3] = new Map( this, IntVec2( MAP_SIZE_X, MAP_SIZE_Y ),map4WormType, map4NumWorm, map4WormLength, map4EdgeType, map4DefaultType );

	m_currentMap = m_maps[m_mapIndex];
	m_texture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/FriendlyTank1.png");
	m_texture1 = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/EnemyTank4.png");
}

void World::Update( float deltaSeconds )
{
	if(m_playerLife < 0){
		m_isRenderGame = false;
		m_isWin = false;
	}
	if(m_currentMap->m_playerExit){
		if(m_mapIndex < NUM_MAP - 1 ){
			m_mapIndex++;
			m_currentMap = m_maps[m_mapIndex];
		}
		else{
			m_isRenderGame = false;
			if(!m_isVictorySceneInitialized){
				InitializeVictoryScene();
			}
			m_isWin = true;
		}
	}
	if(m_isRenderGame) {
		m_currentMap->Update( deltaSeconds );
	}
	else if(m_isWin){
		UpdatePlayerVictoryScene(deltaSeconds);
	}
}

void World::Render()const
{
	if(!m_isRenderGame) {return;}
	m_currentMap->Render();

}



void World::DestroyEntities()
{
	m_currentMap->DestroyEntities();
}

void World::EnterLoseScene()
{
	g_theRenderer->BindTexture( m_texture1 );
}

void World::EnterWinScene()
{

}

void World::RenderUI() const
{
	if(m_isRenderGame){

		AABB2 m_tankShape = AABB2( Vec2( -.5f, -.5f ), Vec2( .5f, .5f ) );
		std::vector<Vertex_PCU> m_tankVertices;
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins.x, m_tankShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs.x, m_tankShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

		const std::vector<Vertex_PCU> m_tankShapeInWorld1 = TransformVertexArray( m_tankVertices, 1, 0, Vec2( 100.5, 108.5 ) );
		const std::vector<Vertex_PCU> m_tankShapeInWorld2 = TransformVertexArray( m_tankVertices, 1, 0, Vec2( 101.5, 108.5 ) );
		const std::vector<Vertex_PCU> m_tankShapeInWorld3 = TransformVertexArray( m_tankVertices, 1, 0, Vec2( 102.5, 108.5 ) );

		g_theRenderer->BindTexture( m_texture );

		switch( m_playerLife )
		{
		case 3:
		{
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld1 );
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld2 );
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld3 );
			break;
		}
		case 2:
		{
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld1 );
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld2 );
			break;
		}
		case 1:
		{
			g_theRenderer->DrawVertexVector( m_tankShapeInWorld1 );
			break;
		}
		case 0:
			break;
		
		default:
			break;
		}

	}
	else{

		AABB2 m_tankShape = AABB2( Vec2( -.5f, -.5f ), Vec2( .5f, .5f ) );
		std::vector<Vertex_PCU> m_tankVertices;
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins.x, m_tankShape.maxs.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 1 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.mins, 0 ), Rgba8( 255, 255, 255 ), Vec2( 0, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs.x, m_tankShape.mins.y, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 0 ) ) );
		m_tankVertices.push_back( Vertex_PCU( Vec3( m_tankShape.maxs, 0 ), Rgba8( 255, 255, 255 ), Vec2( 1, 1 ) ) );

		if(!m_isWin){
			g_theRenderer->BindTexture(m_texture1);
		}
		else{
			RenderPlayerVictoryScene();
			g_theRenderer->BindTexture(m_texture);
		}
		const std::vector<Vertex_PCU> m_tankShapeInWorld1 = TransformVertexArray( m_tankVertices, 1, 0, Vec2( 108, 104.5 ) );
		g_theRenderer->DrawVertexVector(m_tankShapeInWorld1);
	}
}

void World::InitializeVictoryScene()
{
	m_victorySceneState = VICTORY_SCENE_FADE_IN;
	m_victorySceneFadeInTime = 0.f;
	m_victorySceneWaitInputTime = 0.f;
	m_isVictorySceneInitialized = true;
}

void World::UpdatePlayerVictoryScene( float deltaSeconds )
{
	switch( m_victorySceneState )
	{
	case VICTORY_SCENE_FADE_IN: {
		m_victorySceneFadeInTime += deltaSeconds;
		m_victoryScene = AABB2(g_UICamera->m_AABB2);
		float temColorAlpha = RangeMapFloat( 0, 0.5, 0, 255, m_victorySceneFadeInTime );
		temColorAlpha = ClampFloat( 0, 255, temColorAlpha );
		int colorAlpha = RoundDownToInt( temColorAlpha );
		m_victorySceneColor = Rgba8( 0, (unsigned char)colorAlpha, (unsigned char)colorAlpha, (unsigned char)colorAlpha );
		if( m_victorySceneFadeInTime >= 0.5 ) {
			m_victorySceneState = VICTORY_SCENE_WAIT_FOR_READY_INPUT;
		}
		break;
	}
	case VICTORY_SCENE_WAIT_FOR_READY_INPUT:{
		m_victorySceneWaitInputTime += deltaSeconds;
		if(m_victorySceneWaitInputTime > 1){
			m_victorySceneState = VICTORY_SCENE_WAIT_FOR_INPUT;
		}
		break;
	}
	case VICTORY_SCENE_WAIT_FOR_INPUT: {
		if( g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_P ) || g_theInputSystem->WasKeyJustPressed( KEYBOARD_BUTTON_ID_ESC )) {
			m_victorySceneState = VICTORY_SCENE_NULL;
			g_theGame->m_gameState = GAME_STATE_ATTRACT;
			g_theGame->m_resetFinish = false;
		}
		break;
	}
	}
}

void World::RenderPlayerVictoryScene() const
{
	g_theRenderer->BindTexture( nullptr );
	g_theRenderer->DrawAABB2D( m_victoryScene, m_victorySceneColor );
	/*if( m_victorySceneState == DEAD_SCENE_WAIT_FOR_INPUT ) {
		g_theRenderer->BindTexture( g_squirrelFont->GetTexture() );
		g_theRenderer->DrawVertexVector( m_deadMessage );
	}*/
}

