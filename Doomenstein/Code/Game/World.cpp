#include "World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/Map.hpp"
#include "Game/TileMap.hpp"

World::World()
{
	//LoadMap();
	m_currentMap = TileMap::CreateTileMap();
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void World::Render()const
{
	m_currentMap->RenderMap();
}

