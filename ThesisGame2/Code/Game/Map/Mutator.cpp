#include "Mutator.hpp"
#include "Game/Map/map.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


Mutator::Mutator( const XmlElement& mutatorElement )
	:MapGenStep( mutatorElement )
{
}

void Mutator::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTiles = map->GetTiles();
	RandomNumberGenerator* mapRNG = map->GetRNG();

	for( int i = 0; i < mapTiles.size(); i++ ) {
		if( map->IsTileOfTypeWithIndex( m_ifType, i )  && (mapRNG->RollPercentChance( m_chancePerTile )) ) {
			map->SetTileTypeWithIndex( i, m_setType );
		}
	}

}
