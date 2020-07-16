#include "Maze.hpp"
#include "Game/Map/Map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

Maze::Maze( Map* map )
{
	m_map = map;
}

bool Maze::isTileOfMaze( IntVec2 tileCoords ) const
{
	for( int i = 0; i < m_tileCoords.size(); i++ ) {
		if( m_tileCoords[i] == tileCoords ) {
			return true;
		}
	}
	return false;
}

IntVec2 Maze::GetRandomTileCoords() const
{
	RandomNumberGenerator* mapRNG = m_map->GetRNG();	
	int randomTileIndex = mapRNG->RollRandomIntLessThan( (int)m_tileCoords.size() );
	return m_tileCoords[randomTileIndex];
}

void Maze::AddMazeTileCoords( IntVec2 tileCoords )
{
	m_tileCoords.push_back( tileCoords );
}

void Maze::SetLayer( int layer )
{
	m_layer = layer;
}
