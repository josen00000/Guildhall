#include "CellularAutomata.hpp"
#include "Game/Map/map.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


CellularAutomata::CellularAutomata( const XmlElement& CellularAutomataElement )
	:MapGenStep(CellularAutomataElement)
{
	m_ifNeighborType	= ParseXmlAttribute( CellularAutomataElement, "ifNeighborType", m_ifNeighborType );
	m_neighborNum		= ParseXmlAttribute( CellularAutomataElement, "numNeighbors", m_neighborNum );
}

bool CellularAutomata::IsNeighborsEnough( Map* map, int tileIndex ) const
{
	IntVec2 tileCoords = map->GetTileCoordsWithTileIndex( tileIndex );
	IntVec2 leftTileCoords	= tileCoords + IntVec2( -1, 0 );
	IntVec2 rightTileCoords = tileCoords + IntVec2( 1, 0 );
	IntVec2 upTileCoords	= tileCoords + IntVec2( 0, 1 );
	IntVec2 downTileCoords	= tileCoords + IntVec2( 0, -1 );

	IntVec2 leftUpTileCoords	= tileCoords + IntVec2( -1, 1 );
	IntVec2 rightUpTileCoords	= tileCoords + IntVec2( 1, 1 );
	IntVec2 leftDownTileCoords	= tileCoords + IntVec2( -1, -1 );
	IntVec2 rightDownTileCoords	= tileCoords + IntVec2( 1, -1 );

	int neighborNum = 0;
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, leftTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, rightTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, upTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, downTileCoords ) ){ neighborNum++; }

	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, leftUpTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, rightUpTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, leftDownTileCoords ) ){ neighborNum++; }
	if( map->IsTileOfTypeInsideWithCoords( m_ifNeighborType, rightDownTileCoords ) ){ neighborNum++; }

	return m_neighborNum.IsIntInRange( neighborNum );
}

void CellularAutomata::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTiles = map->GetTiles();
	static std::vector<bool> ifChangingTiles = std::vector<bool>( mapTiles.size(), false );
	RandomNumberGenerator* mapRNG = map->GetRNG();

	for( int i = 0; i < mapTiles.size(); i++ ) {
		IntVec2 tileCoords = map->GetTileCoordsWithTileIndex( i );
		if( IsNeighborsEnough( map, i ) && !map->IsTileEdge( tileCoords ) ) {
			ifChangingTiles[i] = true;
		}
		else {
			ifChangingTiles[i] = false;
		}
	}

	for( int i = 0; i < mapTiles.size(); i++ ) {
		if( ifChangingTiles[i] ) {
			map->SetTileType( i, m_setType );
		}
	}
}

