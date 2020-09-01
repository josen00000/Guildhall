#include "MapGenStep.hpp"
#include "Game/Map/map.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"


MapGenStep::MapGenStep( const XmlElement& mapGenElement )
{
	m_chanceToRun		= ParseXmlAttribute( mapGenElement, "chanceToRun", m_chanceToRun );
	m_iterations		= ParseXmlAttribute( mapGenElement, "iterations", m_iterations );
	m_chancePerTile		= ParseXmlAttribute( mapGenElement, "chancePerTile", m_chancePerTile );
	m_ifType			= ParseXmlAttribute( mapGenElement, "ifType", m_ifType );
	m_setType			= ParseXmlAttribute( mapGenElement, "setType", m_setType );
}

void MapGenStep::RunStep( Map* map ) const
{
	RandomNumberGenerator* mapRNG = map->GetRNG();
	if( !mapRNG->RollPercentChance( m_chanceToRun ) ) {
		return;
	}

	for( int i = 0; i < m_iterations; i++ ) {
		RunStepOnce( map );
	}
}

void MapGenStep::ModifyTileType( Map* map, IntVec2 tileCoords ) const
{
	const Tile& tempTile = map->GetTileWithTileCoords( tileCoords );

	if( !tempTile.IsType( m_ifType ) ) {
		return;
	}

	map->SetTileTypeWithCoords( tileCoords, m_setType );
}
