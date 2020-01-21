#pragma once
#include "Game/Entity.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Texture;


class Actor : public Entity
{
public:
	Actor(){}
	~Actor(){}
	explicit Actor( Map* map, Vec2 spawnPosition, ActorDefinition* actorDef );
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;

private:
	void UpdateByXboxInput(float deltaSeconds);
	void CreateActorShapes();
public:
	std::string m_name;
	ActorDefinition* m_definition;
	AABB2 m_shape;
	std::vector <Vertex_PCU> m_vertices;
	//Texture* m_PlayerTexture=nullptr;
};

