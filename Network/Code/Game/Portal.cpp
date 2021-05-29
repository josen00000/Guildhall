#include "Portal.hpp"
#include "Game/Entity.hpp"

Portal::Portal( const EntityDefinition& entityDef )
	:Entity::Entity( entityDef )
{

}

void Portal::SetTarget2DPosition( Vec2 pos )
{
	m_target2DPos = pos;
}

void Portal::SetTargetMapName( std::string mapName )
{
	m_targetMapName = mapName;
}
