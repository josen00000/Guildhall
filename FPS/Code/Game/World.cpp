#include "World.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/Map.hpp"
World::World()
{
	m_currentMap = new Map( this, IntVec2( MAP_SIZE_X, MAP_SIZE_Y ) );
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void World::Render()const
{
	m_currentMap->Render();

}



void World::DestroyEntities()
{
	m_currentMap->DestroyEntities();
}

