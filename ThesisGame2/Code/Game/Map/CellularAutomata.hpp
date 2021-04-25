#pragma once
#include "Game/Map/MapGenStep.hpp"
#include "Engine/Math/IntRange.hpp"


class CellularAutomata : public MapGenStep {
public:
	CellularAutomata(){}
	~CellularAutomata(){}
	explicit CellularAutomata( const XmlElement& CellularAutomataElement );

public:
	bool IsNeighborsEnough( Map* map, int tileIndex ) const;
	virtual void RunStepOnce( Map* map ) const override;

public:
	TileType m_ifNeighborType = "";
	IntRange m_neighborNum = IntRange( 0 );
};
