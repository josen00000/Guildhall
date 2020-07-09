#include "World.hpp"
#include "Game/Map/Map.hpp"
#include "Game/Map/MapDefinition.hpp"

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
	m_maps.clear();
	for( int i = 0; i < m_totalMapIndex; i++ ) {
		Map* temMap = Map::CreateMap( "level1", MapDefinition::s_definitions["level1"] );
		m_maps.push_back( temMap );
	}
}

void World::UpdateWorld( float deltaSeconds )
{
	m_maps[m_currentMapIndex]->UpdateMap( deltaSeconds );
}

void World::RenderWorld() const
{
	m_maps[m_currentMapIndex]->RenderMap();
}

Map* World::GetCurrentMap()
{
	return m_maps[m_currentMapIndex];
}

void World::SetTotalMapIndex( int totalMapIndex )
{
	m_totalMapIndex = totalMapIndex;
}
