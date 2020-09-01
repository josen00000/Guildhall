#pragma once
#include <string>
#include "Game/Entity.hpp"


class Portal : public Entity {
public:	
	Portal( const EntityDefinition& entityDef );
	~Portal(){}

public:
	// Accessor
	Vec2 GetTarget2DPosition() const { return m_target2DPos; }
	std::string GetTargetMapName() const { return m_targetMapName; }

	// Mutator
	void SetTarget2DPosition( Vec2 pos );
	void SetTargetMapName( std::string mapName );
private:
	std::string m_targetMapName;
	Vec2 m_target2DPos = Vec2::ZERO;
};