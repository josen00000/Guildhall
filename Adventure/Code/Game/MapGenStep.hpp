#pragma once
#include "Game/Map.hpp"
#include "Game/Tile.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/FloatRange.hpp"

class MapGenStep
{
public:
	MapGenStep()=default;
	~MapGenStep()=default;
	explicit MapGenStep( const XmlElement& mapGenElement );

	void RunStep( Map* map ) const;
	virtual void RunStepOnce( Map* map ) const = 0;
	virtual void ModifyTile( Map* map, IntVec2 tileCoords ) const;

public:
	float		m_chanceToRun	= 1;
	int			m_iteration		= 1;
	float		m_chancePerTile = 1;
	std::string m_ifType;
	std::string m_setType;
	Strings		m_setTags;
	Strings		m_ifTagContain;
	FloatRange	m_setHeat		= FloatRange( 1, 1 );
	FloatRange	m_ifHeatWithin	= FloatRange( MINIMUM_HEAT_RANGE, MAXIMUM_HEAT_RANGE );
	FloatRange	m_operateHeat	= FloatRange( 1, 1 );
	int			m_heatOperation	= 0; // 0 for add, 1 for subtract, 2 for multiply, 3 for divide

};


