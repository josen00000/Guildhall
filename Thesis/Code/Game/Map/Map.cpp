#include "Map.hpp"
#include <stack>
#include <time.h>
#include "Game/Player.hpp"
#include "Game/Item.hpp"
#include "Game/Game.hpp"
#include "Game/Map/MapDefinition.hpp"
#include "Game/Map/MapGenStep.hpp"
#include "Game/Map/TileDefinition.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time/Timer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Physics/Physics2D.hpp"
#include "Engine/Physics/RigidBody2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/MeshUtils.hpp"
#include "Engine/Renderer/RenderContext.hpp"


//////////////////////////////////////////////////////////////////////////
// Global variable
//////////////////////////////////////////////////////////////////////////

// Game
//////////////////////////////////////////////////////////////////////////
extern Game*			g_theGame;

// Engine
//////////////////////////////////////////////////////////////////////////
extern AudioSystem*		g_theAudioSystem;
extern Camera*			g_debugCamera;
extern Camera*			g_gameCamera;
extern InputSystem*		g_theInputSystem;
extern RenderContext*	g_theRenderer;
extern Physics2D*		g_thePhysics;


Map::Map( std::string name, MapDefinition* definition )
	:m_name(name)
	,m_definition(definition)
{
	static int seed = 0;
	m_rng = new RandomNumberGenerator( seed );
	m_width = m_definition->GetWidth();
	m_height = m_definition->GetHeight();
	seed++;
}

Map* Map::CreateMap( std::string name, MapDefinition* definition )
{
	Map* newMap = new Map( name, definition );
	return newMap;
}

// Accessor
//////////////////////////////////////////////////////////////////////////
bool Map::IsTileOfTypeInsideWithCoords( TileType type, IntVec2 tileCoords ) const
{
	return true;
}

bool Map::IsTileOfTypeWithIndex( TileType type, int index ) const
{
	return true;
}

bool Map::IsTileCoordsInside( IntVec2 tileCoords ) const
{
	return true;
}

IntVec2 Map::GetTileCoordsWithTileIndex( int index ) const
{
	return IntVec2( 0 );
}

int Map::GetTileIndexWithTileCoords( IntVec2 tileCoords ) const
{
	return 0;
}

Tile& Map::GetTileWithTileCoords( IntVec2 coords )
{
	return m_tiles[0];
}
//////////////////////////////////////////////////////////////////////////


// Mutator
//////////////////////////////////////////////////////////////////////////
void Map::SetTileTypeWithCoords( IntVec2 coords, TileType type )
{

}

void Map::SetTileTypeWithIndex( int index, TileType type )
{

}
//////////////////////////////////////////////////////////////////////////


// Basic
//////////////////////////////////////////////////////////////////////////
void Map::UpdateMap( float deltaSeconds )
{
	// generate map
	// update map
	if( m_isGeneratingMap ) {
		m_tilesVertices.clear();
		// initialize
		// Generate rooms
		// Generate maze
		// Eliminate dead end
		static int stepIndex = 0;
		switch( stepIndex )
		{
		case 0:
			InitializeTiles();
			PopulateTiles();
			stepIndex++;
			break;
		case 1:
			GenerateRooms();
			PopulateTiles();
			stepIndex++;
			break;	
		case 2:
			GenerateMazes();
			PopulateTiles();
			stepIndex++;
		case 3:
			m_isGeneratingMap = false;

		default:
			break;
		}
	}

	if( g_theGame->GetIsDebug() ) {
		
	}
}

void Map::RenderMap()
{
	g_theRenderer->SetDiffuseTexture( TileDefinition::s_spriteTexture );
	g_theRenderer->DrawVertexVector( m_tilesVertices );
}
//////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////
// private
//////////////////////////////////////////////////////////////////////////

// Generate map
//////////////////////////////////////////////////////////////////////////
void Map::LoadMapDefinitions()
{
	m_width		= m_definition->GetWidth();
	m_height	= m_definition->GetHeight();
	
	m_tiles.clear();
	m_tiles.reserve( m_width * m_height );
}

void Map::PopulateTiles()
{
	m_tilesVertices.clear();
	m_tilesVertices.reserve( m_tiles.size() * 4 );
	for( int i = 0; i < m_tiles.size(); i++ ) {
		Tile& tempTile = m_tiles[i];
		TileDefinition* tempTileDef = tempTile.GetTileDef();
		AABB2 tileBounds = tempTile.GetTileBounds();
		Vec2 tileUVAtMin = tempTileDef->GetSpriteUVAtMins();
		Vec2 tileUVAtMax = tempTileDef->GetSpriteUVAtMaxs();

		AppendVertsForAABB2D( m_tilesVertices, tileBounds, Rgba8::WHITE, tileUVAtMin, tileUVAtMax );
	}
}

void Map::InitializeTiles()
{
	for( int i = 0; i < m_width; i++ ) {
		for( int j = 0; j < m_height; j++ ) {
			m_tiles.emplace_back( IntVec2( i, j ), m_definition->GetEdgeType() );
		}
	}
}

void Map::GenerateRooms()
{
	const std::vector<MapGenStep*>& mapSteps = m_definition->GetMapGenSteps();
	for( int i = 0; i < mapSteps.size(); i++ ) {
		mapSteps[i]->RunStep( this );
	}
}

void Map::GenerateMazes()
{
	// find start tiles
	// use DFS explore mazes.
}
//////////////////////////////////////////////////////////////////////////


// Private Update
//////////////////////////////////////////////////////////////////////////
void Map::UpdateDebugInfo()
{
	// Get cursor info
	// Get debug tile
	// Set Debug Info
	HWND handle = ->
	Vec2 cursorPos = g_theInputSystem->GetNormalizedMousePosInCamera()
}
//////////////////////////////////////////////////////////////////////////
