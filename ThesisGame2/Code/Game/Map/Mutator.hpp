#pragma once
#include "Game/Map/MapGenStep.hpp"

class Mutator : public MapGenStep {
public:
	Mutator(){}
	~Mutator(){}
	explicit Mutator( const XmlElement& mutatorElement );

public:
	virtual void RunStepOnce( Map* map ) const override;
};