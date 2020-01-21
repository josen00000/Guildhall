#pragma once
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/IntRange.hpp"

class PerlinNoise : public MapGenStep
{
public:
	PerlinNoise()=default;
	~PerlinNoise()=default;
	explicit PerlinNoise( const XmlElement& perlinElement );
	virtual void RunStepOnce( Map* map ) const override;

public:
	FloatRange m_ifNoiseWithin = FloatRange( 0, 1 );
	IntRange m_gridSize = IntRange( 3, 5 );
	IntRange m_numOctaves = IntRange( 2, 4 );
	float m_persistence = 0.5f;
	int m_octaveScale = 2;
	int m_seed = 0;
};