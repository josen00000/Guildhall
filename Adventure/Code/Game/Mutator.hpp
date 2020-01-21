#pragma once
#include "Game/MapGenStep.hpp"
#include "Game/TileDefinition.hpp"

class Mutator : public MapGenStep
{
public:
	Mutator()=default;
	~Mutator()=default;
	explicit Mutator( const XmlElement& mutatorElement );
	virtual void RunStepOnce( Map* map ) const override;

};