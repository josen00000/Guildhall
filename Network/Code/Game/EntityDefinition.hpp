#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Game/MaterialsSheet.hpp"

class SpriteAnimDefinition;

class EntityDefinition;
typedef std::map<std::string, EntityDefinition> EntityTypeMap;
typedef	std::vector<SpriteAnimDefinition*> AnimationsSpriteDefinitions;
typedef std::string AnimationType;
using EntityType = std::string;

enum MoveDirection: int {
	MOVE_FRONT = 0,
	MOVE_LEFT,
	MOVE_RIGHT,
	MOVE_BACK,
	MOVE_FRONT_LEFT,
	MOVE_FRONT_RIGHT,
	MOVE_BACK_LEFT,
	MOVE_BACK_RIGHT,
	NUM_MOVE_DIRECTION
};

class EntityDefinition {
public:
	EntityDefinition(){}
	~EntityDefinition(){}

	static void LoadEntityDefinitions( const XmlElement& EntityTypesElement );
	static EntityTypeMap s_definitions;

	explicit EntityDefinition( const XmlElement& entityElement );

public:
	const AnimationsSpriteDefinitions GetAnimationSpriteDefinitions( AnimationType animationType ) const;
	Vec2 GetMoveDirtNormal( MoveDirection moveDirt ) const;
	Texture* GetSpriteTexture() const;
private:
	std::string GetStringWithMoveDirection( MoveDirection moveDirt );
public:
	// physics
	float		m_radius = 0.1f;
	float		m_height = 1.f;
	float		m_eyeHeight = 0.f;
	float		m_walkSpeed = 1.f;

	// name
	std::string m_name = "";
	std::string m_type = "";

	// Apperance
	Vec2			m_size				= Vec2::ZERO;
	std::string		m_billboardType		= "";
	std::string		m_spriteSheetPath	= "";
	IntVec2			m_spriteSheetLayout = IntVec2::ZERO;
	SpriteSheet*	m_spriteSheet		= nullptr;
	Texture*		m_spriteTexture		= nullptr;
	std::map<AnimationType, AnimationsSpriteDefinitions> m_spriteAnimations;
};