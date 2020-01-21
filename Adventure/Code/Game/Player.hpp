#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "engine/Math/AABB2.hpp"
#include "Engine/Audio/AudioSystem.hpp"

class Texture;


class Player : public Entity
{
public:
	Player(){}
	~Player();
	explicit Player(Map* map, Vec2 spawnPosition);
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;

private:
	void CreatePlayer();
	void UpdatePlayer( float deltaSeconds );
	void RenderPlayer() const;

public:
	AABB2 m_PlayerShape;
	std::vector <Vertex_PCU> m_PlayerVertices;
	Texture* m_PlayerTexture=nullptr;
};

