#pragma once
#include <string>
#include "Engine/Core/XmlUtils.hpp"

class Map;

typedef std::string TileType;


class MapGenStep {
public:
	MapGenStep(){}
	~MapGenStep(){}
	explicit MapGenStep( const XmlElement& mapGenElement );

public:
	void RunStep( Map* map ) const;
	virtual void RunStepOnce( Map* map ) const = 0;
	virtual void ModifyTileType( Map* map, IntVec2 tileCoords ) const;

	// Accessor
	float GetChanceToRun() const { return m_chanceToRun; }
	int GetIteration() const { return m_iterations; }
	float GetChancePerTile() const { return m_chancePerTile; }

protected:
	float		m_chanceToRun = 1.f;
	int			m_iterations = 1;
	float		m_chancePerTile = 1.f;
	TileType	m_ifType = "";
	TileType	m_setType = "";
};