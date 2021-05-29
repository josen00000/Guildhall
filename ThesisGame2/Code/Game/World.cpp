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
	m_maps.reserve( m_totalMapIndex );
	Map* temMap = Map::CreateMap( "level1", MapDefinition::s_definitions["level1"] );
	for( int i = 0; i < 4; i++ ) {
		temMap->CreatePlayer();
	}
	m_maps.push_back( temMap );
}

void World::UpdateWorld( float deltaSeconds )
{
	if( m_currentMapIndex == 0 ) {
		
	}
	m_maps[m_currentMapIndex]->UpdateMap( deltaSeconds );
}

void World::RenderWorld( int controllerIndex ) const
{
	m_maps[m_currentMapIndex]->RenderMap( controllerIndex );
}

void World::EndFrame()
{
	m_maps[m_currentMapIndex]->EndFrame();
}

Map* World::GetCurrentMap()
{
	return m_maps[m_currentMapIndex];
}

void World::SetTotalMapIndex( int totalMapIndex )
{
	m_totalMapIndex = totalMapIndex;
}

void World::SetCurrentMapIndex( int currentMapIndex )
{
	m_currentMapIndex = currentMapIndex;
}
