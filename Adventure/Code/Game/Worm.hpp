#pragma once
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/IntRange.hpp"

class Worm: public MapGenStep
{
public:
	Worm()=default;
	~Worm()=default;
	explicit Worm( const XmlElement& wormElement );
	virtual void RunStepOnce( Map* map ) const override;
	IntVec2 FindStartPos( Map* map );
private:
	IntVec2 GetNextWormPos( RandomNumberGenerator& rng, const IntVec2& currentPos ) const;
public:
	IntRange	m_wormLength		= IntRange( 1 );
	IntRange	m_numWorm			= IntRange( 1 );
	FloatRange	m_startHeatWithin	= FloatRange( 0.f );
	std::string m_startType			= "";
	Strings		m_startTagsInclude;
};