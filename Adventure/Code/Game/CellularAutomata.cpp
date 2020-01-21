#include "CellularAutomata.hpp"
#include "Engine/Core/XmlUtils.hpp"

CellularAutomata::CellularAutomata( const XmlElement& automataElement )
	:MapGenStep(automataElement)
{
	m_ifNeighborType				= ParseXmlAttribute( automataElement, "ifNeighborType", "Grass" );
 	m_neighborNum					= ParseXmlAttribute( automataElement, "numNeighbors", IntRange( 1,5 ) ); 
}

void CellularAutomata::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTiles = map->GetMapTiles();
	std::vector<bool> Checktiles;
	Checktiles.resize( mapTiles.size() );	
	//RandomNumberGenerator& mapRNG = map->m_rng;

	for(int tileIndex = 0; tileIndex < mapTiles.size(); tileIndex++ ){
		Checktiles[tileIndex] = false;
		IntVec2 tileCoords = map->GetTileCoordsWithTileIndex( tileIndex );
		if( IsEnoughNeighbors( map, tileCoords)){
			Checktiles[tileIndex] = true;
		}
	}
	for(int tileIndex = 0; tileIndex < Checktiles.size(); tileIndex++ ){
		if(Checktiles[tileIndex] == true){
			// TODO add modify tile function			
		}
	}
}

bool CellularAutomata::IsEnoughNeighbors( Map* map, const IntVec2 tileCoords ) const
{
	IntVec2 topNeighborCoords	= tileCoords + IntVec2( 0, 1 );
	IntVec2 downNeighborCoords	= tileCoords + IntVec2( 0, -1 );
	IntVec2 rightNeighborCoords = tileCoords + IntVec2( 1, 0 );
	IntVec2 leftNeighborCoords	= tileCoords + IntVec2( -1, 0 );

	IntVec2 topLeftNeighborCoords	= tileCoords + IntVec2( -1, 1 );
	IntVec2 topRightNeighborCoords	= tileCoords + IntVec2( 1, 1 );
	IntVec2 downLeftNeighborCoords	= tileCoords + IntVec2( -1, -1 );
	IntVec2 downRightNeighborCoords	= tileCoords + IntVec2( 1, -1 );

	int neighborNum = 0;
	if( map->IsTileInType( topNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( downNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( leftNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( rightNeighborCoords, m_ifNeighborType ) )	{ neighborNum++; }
	
	if( map->IsTileInType( topLeftNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( topRightNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( downLeftNeighborCoords, m_ifNeighborType ) )		{ neighborNum++; }
	if( map->IsTileInType( downRightNeighborCoords, m_ifNeighborType ) )	{ neighborNum++; }

	if( m_neighborNum.IsIntInRange( neighborNum )){
		return true;
	}
	else{
		return false;
	}
}
