#include "World.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"

World::~World()
{

}

World* World::CreateWorld( int totalMapIndex )
{
	World* result = new World();
	result->SetTotalMapIndex( totalMapIndex );
	result->CreateMaps();
	return result;
}

void World::CreateMaps()
{
	for( int i = 0; i < m_totalMapIndex; i++ ) {
		Map::CreateMap( "dungeon", MapDefinition::s_definitions["dungeon"] );
	}
}

void World::RenderWorld() const
{
	m_maps[m_currentMapIndex]->RenderMap();
}

void World::SetTotalMapIndex( int totalMapIndex )
{
	m_totalMapIndex = totalMapIndex;
}
