#pragma once
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"

class CellularAutomata: public MapGenStep
{
public:
	CellularAutomata()=default;
	~CellularAutomata()=default;
	explicit CellularAutomata( const XmlElement& wormElement );
	virtual void RunStepOnce( Map* map ) const override;
	bool IsEnoughNeighbors( Map* map, const IntVec2 tileCoords ) const;
public:
	std::string m_ifNeighborType = "";
	IntRange m_neighborNum = IntRange( 1, 999 );
};