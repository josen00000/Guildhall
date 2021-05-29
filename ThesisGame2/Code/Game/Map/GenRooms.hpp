#pragma once
#include "Game/Map/MapGenStep.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/IntVec2.hpp"

class GenRooms : public MapGenStep {
public:
	GenRooms(){}
	~GenRooms(){}
	explicit GenRooms( const XmlElement& roomElement );

public:
	virtual void RunStepOnce( Map* map ) const override;

private:
	void CreateRoom( Map* map, RandomNumberGenerator rng ) const;
public:
	bool			m_isPosRandom = false;
	IntRange		m_widthRange	= IntRange( 0 );
	IntRange		m_heightRange	= IntRange( 0 );
	IntRange		m_roomsNumRange = IntRange( 0 );
	IntVec2			m_pos = IntVec2::ZERO;
	TileType		m_floorType = "";
	TileType		m_wallType = "";
	TileType		m_doorType = "";
};