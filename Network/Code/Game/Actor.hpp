#pragma once
#include "Game/Entity.hpp"
#include "Engine/Math/vec2.hpp"

class Actor : public Entity {
public:	
	Actor(){}
	~Actor(){}
	explicit Actor( const EntityDefinition& entityDef );

public:


public:
	virtual void Update( float deltaSeconds ) override;
public:
	std::string m_animationType = "Walk";
	MoveDirection m_currentMoveDirt;
};