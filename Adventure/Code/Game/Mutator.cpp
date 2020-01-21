#include "Mutator.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Game/TileDefinition.hpp"

Mutator::Mutator( const XmlElement& mutatorElement )
	:MapGenStep(mutatorElement)
{
}

void Mutator::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTile = map->GetMapTiles();
	RandomNumberGenerator& mapRNG = map->m_rng;
	for( int tileIndex = 0; tileIndex < mapTile.size(); tileIndex++ ) {
		Tile& tempTile = mapTile[tileIndex];
		if( tempTile.m_type == m_ifType ) {
			if( mapRNG.RollPercentChance( m_chancePerTile ) ) {
				if( tempTile.IsTagsExist( m_ifTagContain )){
					tempTile.SetTileType( m_setType );
					tempTile.SetTileTags( m_setTags );
				}
			}
		}
	}
}
