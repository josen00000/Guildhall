#include "Maze.hpp"

bool Maze::isTileOfMaze( IntVec2 tileCoords ) const
{
	for( int i = 0; i < m_tileCoords.size(); i++ ) {
		if( m_tileCoords[i] == tileCoords ) {
			return true;
		}
	}
	return false;
}

void Maze::AddMazeTileCoords( IntVec2 tileCoords )
{
	m_tileCoords.push_back( tileCoords );
}

void Maze::SetLayer( int layer )
{
	m_layer = layer;
}
