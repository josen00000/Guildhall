#pragma once
#include "Game/Map/MapGenStep.hpp"

typedef	std::pair<std::string, std::string> PerlinPair;

class PerlinScroll : public MapGenStep {
public:
	PerlinScroll(){}
	~PerlinScroll(){}
	explicit PerlinScroll( const XmlElement& perlinScrollElement );

public:
	virtual void RunStepOnce( Map* map ) const override;

private:
	std::vector<PerlinPair> m_perlinTilePairs;
};