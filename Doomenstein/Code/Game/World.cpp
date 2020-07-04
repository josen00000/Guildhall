#include "World.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/Map.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileMap.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"

extern Game* g_theGame;

World::World()
{
	//LoadMap();
	//CreateMaps();
	g_theConsole->AddCommandToCommandList( std::string( "raycast" ), std::string( "add raycast with start pos and end pos" ), MapCommandRaycast );
}

void World::Update( float deltaSeconds )
{
	m_maps[m_currentMapIndex]->Update( deltaSeconds );
}

void World::Render()const
{
	m_maps[m_currentMapIndex]->RenderMap();
}

void World::AddMap( Map* map )
{
	m_maps.push_back( map );
}

bool World::LoadMap( std::string mapName )
{
	mapName = mapName + ".xml";
	bool isValidMap = false;
	for( int i = 0; i < m_maps.size(); i++ ) {
		if( m_maps[i]->m_name == mapName ){ 
			m_currentMapIndex = i;
			isValidMap = true;
			m_maps[i]->PreparePlayer();
		}
	}
	return isValidMap;
}

Strings World::GetAllMaps() const
{
	Strings resultMaps;
	for( int i = 0; i < m_maps.size(); i++ ) {
		resultMaps.push_back( m_maps[i]->m_name );
	}
	return resultMaps;
}

Map* World::GetCurrentMap()
{
	return m_maps[m_currentMapIndex];
}

void World::SetDebugMode( bool isDebug )
{
	m_isDebugMode = isDebug;
	m_maps[m_currentMapIndex]->SetDebugMode( isDebug );
}

bool MapCommandRaycast( EventArgs& args )
{
	Vec3 startPos = args.GetValueByString( std::to_string( 0 ), Vec3::ZERO );
	Vec3 endPos = args.GetValueByString( std::to_string( 1 ), Vec3::ONE );
	Map* currentMap = g_theGame->m_world->GetCurrentMap();
	currentMap->RayCast( startPos, endPos );
	return true;
}
