#include "MapGenStep.hpp"

MapGenStep::MapGenStep( const XmlElement& mapGenElement )
{
	m_chanceToRun		= ParseXmlAttribute( mapGenElement, "chanceToRun", 1.f);
	m_iteration			= ParseXmlAttribute( mapGenElement, "iterations", 1);
	m_chancePerTile		= ParseXmlAttribute( mapGenElement, "chancePerTile", 1.f);
	
	m_ifType			= ParseXmlAttribute( mapGenElement, "ifType", "Grass"); 
	m_setType			= ParseXmlAttribute( mapGenElement, "setType", "Grass" );
	
	m_setTags			= ParseXmlAttribute( mapGenElement, "setTags", m_setTags, "," );
	m_ifTagContain		= ParseXmlAttribute( mapGenElement, "ifTagsContain", m_ifTagContain, "," );
	
	m_setHeat			= ParseXmlAttribute( mapGenElement, "setHeat", m_setHeat );
	m_ifHeatWithin		= ParseXmlAttribute( mapGenElement, "ifHeatWithin", m_ifHeatWithin );
	m_operateHeat		= ParseXmlAttribute( mapGenElement, "operateHeat", m_operateHeat );
	m_heatOperation		= ParseXmlAttribute( mapGenElement, "heatOperation", m_heatOperation );
	
}

void MapGenStep::RunStep( Map* map ) const
{
	if( !map->m_rng.RollPercentChance( m_chanceToRun )){
		return;
	}

	for(int i = 0; i < m_iteration; ++i ){
		RunStepOnce( map );
	}
}

void MapGenStep::ModifyTile( Map* map, IntVec2 tileCoords ) const
{
	Tile& tempTile = map->GetTileWithTileCoords( tileCoords );
	if( !tempTile.IsTileType( m_ifType ) ){
		return;
	}
	if( !tempTile.IsTagsExist( m_ifTagContain ) ){
		return;
	}
	if( !tempTile.IsHeatBetween( m_ifHeatWithin ) ){
		return;
	}

	map->SetTileType( tileCoords.x, tileCoords.y, m_setType );
	map->SetTileTags( tileCoords.x, tileCoords.y, m_setTags );
	map->SetTileHeat( tileCoords.x, tileCoords.y, m_setHeat );
	map->OperateTileHeat( tileCoords.x, tileCoords.y, m_heatOperation, m_operateHeat );
}

