#include "PerlinNoise.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Game/TileDefinition.hpp"

PerlinNoise::PerlinNoise( const XmlElement& perlinElement )
	:MapGenStep(perlinElement)
{
	m_ifNoiseWithin	= ParseXmlAttribute( perlinElement, "ifNoiseWithin", m_ifNoiseWithin );
	m_gridSize		= ParseXmlAttribute( perlinElement, "gridSize", m_gridSize );
	m_numOctaves	= ParseXmlAttribute( perlinElement, "numOctaves", m_numOctaves );
	m_persistence	= ParseXmlAttribute( perlinElement, "persistence", m_persistence );
	m_octaveScale	= ParseXmlAttribute( perlinElement, "octaveScale", m_octaveScale );
	m_seed			= ParseXmlAttribute( perlinElement, "seed", m_seed ); 
}

void PerlinNoise::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTile = map->GetMapTiles();
	RandomNumberGenerator& mapRNG = map->m_rng;
	for(int tileIndex = 0; tileIndex < mapTile.size(); tileIndex++){
		IntVec2 tileCoords = map->GetTileCoordsWithTileIndex( tileIndex );
		float noise = (float)Compute2dPerlinNoise( (float)tileCoords.x, (float)tileCoords.y, (float)m_gridSize.GetRandomInRange( mapRNG ), (unsigned int)m_numOctaves.GetRandomInRange( mapRNG ), m_persistence, (float)m_octaveScale, true, m_seed );
		if( m_ifNoiseWithin.IsFloatInRange( noise )){
			map->SetTileType( tileCoords.x, tileCoords.y, m_setType );
		}
	}
}
