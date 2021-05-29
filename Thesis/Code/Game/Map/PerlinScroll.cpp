#include "PerlinScroll.hpp"
#include "Game/Map/map.hpp"
#include "Game/Map/Tile.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/ThirdParty/XmlUtils.hpp"
#include "Engine/Math/SmoothNoise.hpp"
#include "Game/Map/TileDefinition.hpp"


PerlinScroll::PerlinScroll( const XmlElement& perlinScrollElement )
	:MapGenStep( perlinScrollElement )
{
	Strings perlinPairsStrings = ParseXmlAttribute( perlinScrollElement, "perlinPairs", Strings(), "," );
	for( std::string perlinPairsStr : perlinPairsStrings ) {
		Strings perlinPairStrings = SplitStringOnDelimiter( perlinPairsStr, "|" );
		m_perlinTilePairs.push_back( PerlinPair( perlinPairStrings[0], perlinPairStrings[1] ));
	}

}

void PerlinScroll::RunStepOnce( Map* map ) const
{
	std::vector<Tile>& mapTiles = map->GetTiles();
	int mapWidth = map->GetWidth();
	int mapHeight = map->GetHeight();
	for( int i = 0; i < mapTiles.size(); i++ ) {
		IntVec2 tileCoords = map->GetTileCoordsWithTileIndex( i );
		float smoothNoize = Compute1dPerlinNoise( (float)tileCoords.x / 30.f );
		int currentPairIndex = (int)(( (float)tileCoords.x / (float)mapWidth ) * (float)m_perlinTilePairs.size() );
		float currentTileHeight = (float)tileCoords.y / (float)mapHeight;
		currentTileHeight = currentTileHeight * 2.f - 1.f;
		std::string tileType = "";
		if( currentTileHeight > smoothNoize ) {
			tileType = m_perlinTilePairs[currentPairIndex].first;
		}
		else {
			tileType = m_perlinTilePairs[currentPairIndex].second;
		}
		mapTiles[i].SetTileType( tileType );
		TileDefinition* tileDef = TileDefinition::s_definitions[tileType];
		map->SetTileWalkable( tileCoords, !tileDef->m_allowsWalk );
	}
	//map->SetStartcoords( IntVec2( 1 , (int)mapWidth / 2 ) );
}
