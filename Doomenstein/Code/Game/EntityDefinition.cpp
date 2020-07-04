#include "EntityDefinition.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

extern DevConsole* g_theConsole;
extern RenderContext* g_theRenderer;

EntityTypeMap EntityDefinition::s_definitions;

void EntityDefinition::LoadEntityDefinitions( const XmlElement& entityTypesElement )
{

// 	if( !IsElementNameValid( entityTypesElement, "EntityTypes" ) ) {
// 		g_theConsole->DebugErrorf( "Root Name Error. %s should be %s", entityTypesElement.Name(), "EntityTypes" );
// 		return;
// 	}

	Strings validNames{ "Actor", "Projectile", "Portal" };
	const XmlElement* entityTypeElement = entityTypesElement.FirstChildElement();

	while( entityTypeElement ) {
// 		if( !IsElementNameValidWithStrings( entityTypesElement, validNames ) ) {
// 			g_theConsole->DebugErrorf( "Element name Error. %s is not correct.", entityTypeElement->Name() );
// 		}
		EntityDefinition entityDef = EntityDefinition( *entityTypeElement );
		s_definitions[entityDef.m_name] = entityDef;
		entityTypeElement = entityTypeElement->NextSiblingElement();
	}
}

EntityDefinition::EntityDefinition( const XmlElement& entityElement )
{
	m_type		= entityElement.Name();
	m_name		= ParseXmlAttribute( entityElement, "name", m_name ); 

	const XmlElement* physicsElement	= entityElement.FirstChildElement();
	const XmlElement* apperanceElement	= physicsElement->NextSiblingElement();

	if( !physicsElement ){
		g_theConsole->DebugErrorf( "No physics element in element:%s", m_name.c_str() );
	}
	else {
		m_radius	= ParseXmlAttribute( *physicsElement, "radius", m_radius );
		m_height	= ParseXmlAttribute( *physicsElement, "height", m_height );
		m_eyeHeight = ParseXmlAttribute( * physicsElement, "eyeHeight", m_eyeHeight );
		m_walkSpeed	= ParseXmlAttribute( *physicsElement, "walkSpeed", m_walkSpeed );
	}
	
	if( apperanceElement ) {
		m_size				= ParseXmlAttribute( *apperanceElement, "size", m_size );
		m_billboardType		= ParseXmlAttribute( *apperanceElement, "billboard", m_billboardType );
		m_spriteSheetPath	= ParseXmlAttribute( *apperanceElement, "spriteSheet", m_spriteSheetPath );
		m_spriteSheetLayout	= ParseXmlAttribute( *apperanceElement, "layout", m_spriteSheetLayout );

		Texture* spriteTexture = g_theRenderer->CreateOrGetTextureFromFile( m_spriteSheetPath.c_str() );
		m_spriteTexture = spriteTexture;
		m_spriteSheet = new SpriteSheet( *spriteTexture, m_spriteSheetLayout );

		const XmlElement* animationElement = apperanceElement->FirstChildElement();
		while( animationElement ) {
			std::string animationType = animationElement->Name();
			AnimationsSpriteDefinitions animationsSpriteDefinitions;
			for( int i = 0; i < NUM_MOVE_DIRECTION; i++ ) {
				std::string moveDirtString = GetStringWithMoveDirection( (MoveDirection)i );
				Strings indexesString = ParseXmlAttribute( *animationElement, moveDirtString.c_str(), Strings(), "," );
				std::vector<int> animationIndexes;
				for( int j = 0; j < indexesString.size(); j++ ) {
					int index = GetIntFromText( indexesString[j].c_str() );
					animationIndexes.push_back( index );
				}
				SpriteAnimDefinition* tempAnimDef;
				if( animationIndexes.size() == 0 ) {
					tempAnimDef = nullptr;
				}
				else {
					tempAnimDef = new SpriteAnimDefinition( *m_spriteSheet, animationIndexes.data(), (int)animationIndexes.size(), 0.1f );
				}
				animationsSpriteDefinitions.push_back( tempAnimDef );
			}
			m_spriteAnimations[animationType] = animationsSpriteDefinitions;
			animationElement = animationElement->NextSiblingElement();
		}
	}
	else {
		g_theConsole->DebugErrorf( "No Apperance element in element:%s", m_name.c_str() );
	}
}

const AnimationsSpriteDefinitions EntityDefinition::GetAnimationSpriteDefinitions( AnimationType animationType ) const
{
	//const AnimationsSpriteDefinitions 
	auto result = m_spriteAnimations.find( animationType );
	if( result != m_spriteAnimations.end() ) {
		return result->second;
	}
	ERROR_RECOVERABLE( "no animation! " );
	return AnimationsSpriteDefinitions();
}

Vec2 EntityDefinition::GetMoveDirtNormal( MoveDirection moveDirt ) const
{
	switch( moveDirt )
	{
	case MOVE_FRONT:
		return Vec2( 1.f, 0.f );
	case MOVE_LEFT:
		return Vec2( 0.f, 1.f );
	case MOVE_RIGHT:
		return Vec2( 0.f, -1.f );
	case MOVE_BACK:
		return Vec2( -1.f, 0.f );
	case MOVE_FRONT_LEFT:
		return Vec2( 0.7071f, 0.7071f );
	case MOVE_FRONT_RIGHT:
		return Vec2( 0.7071f, -0.7071f );
	case MOVE_BACK_LEFT:
		return Vec2( -0.7071f, 0.7071f );
	case MOVE_BACK_RIGHT:
		return Vec2( -0.7071f, -0.7071f );
	}
	ERROR_RECOVERABLE(" wrong direction ! ");
	return Vec2::ZERO;
}

Texture& EntityDefinition::GetSpriteTexture() const
{
	return m_spriteSheet->GetTexture();
}

std::string EntityDefinition::GetStringWithMoveDirection( MoveDirection moveDirt )
{
	switch( moveDirt )
	{
	case MOVE_FRONT:
		return "front";
	case MOVE_LEFT:
		return "left";
	case MOVE_RIGHT:
		return "right";
	case MOVE_BACK:
		return "back";
	case MOVE_FRONT_LEFT:
		return "frontLeft";
	case MOVE_FRONT_RIGHT:
		return "frontRight";
	case MOVE_BACK_LEFT:
		return "backLeft";
	case MOVE_BACK_RIGHT:
		return "backRight";
	}
	ERROR_RECOVERABLE( "invalid move direction" );
	return "";
}
