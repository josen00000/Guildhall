#include "Worm.hpp"
#include "Engine/Core/XmlUtils.hpp"

Worm::Worm( const XmlElement& wormElement )
	:MapGenStep(wormElement)
{
	m_wormLength		= ParseXmlAttribute( wormElement, "wormLength", IntRange( 3, 5) );
	m_numWorm			= ParseXmlAttribute( wormElement, "numWorm", IntRange( 3, 5 ) );
	m_startHeatWithin	= ParseXmlAttribute( wormElement, "startHeatWithin", m_startHeatWithin );
	m_startType			= ParseXmlAttribute( wormElement, "startType", m_startType );
	m_startTagsInclude	= ParseXmlAttribute( wormElement, "startTagsInclude", m_startTagsInclude, "," );
}

void Worm::RunStepOnce( Map* map ) const
{
	//std::vector<Tile>& mapTiles = map->GetMapTiles();
	RandomNumberGenerator& temRng = map->m_rng;
	int numWorm = m_numWorm.GetRandomInRange( temRng );
	for(int wormIndex = 0; wormIndex < numWorm; ++wormIndex){
		int wormLength = m_wormLength.GetRandomInRange( temRng );
		IntVec2 nextWormPos = map->GetRandomValidTilePositionInMap();
		while( wormLength > 0 ) {
			ModifyTile( map, nextWormPos );
			wormLength--;
			nextWormPos = GetNextWormPos( temRng, nextWormPos );
			if( !map->IsTileInside( nextWormPos ) ) { break; }
		}
	}
}


IntVec2 Worm::FindStartPos( Map* map )
{
	IntVec2 startPos;
	while( 1 ){
		startPos = map->GetRandomValidTilePositionInMap();
		Tile& temTile = map->GetTileWithTileCoords( startPos );
	}
}

IntVec2 Worm::GetNextWormPos( RandomNumberGenerator& rng, const IntVec2& currentPos ) const
{
	int direction = rng.RollRandomIntInRange( 0, 3 );
	IntVec2 nextPos;
	switch( direction )
	{
	case 0:
		nextPos = currentPos + IntVec2( 1, 0 );
		break;
	case 1:
		nextPos = currentPos + IntVec2( -1, 0 );
		break;
	case 2:
		nextPos = currentPos + IntVec2( 0, 1 );
		break;
	case 3:
		nextPos = currentPos + IntVec2( 0, -1 );
		break;
	}
	return nextPos;

}
