#pragma once
#include <vector>
#include "Game/Entity.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Renderer/Texture.hpp"

class Texture;

class Bullet : public Entity
{
public:
	Bullet(){}
	~Bullet();
	explicit Bullet(Map* map, Vec2 iniPos, EntityType entityType, EntityFaction entityFaction, Vec2 fwdDir = Vec2(1.f,  0.f) );

	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;

private:
	void CreateBullet();

public:
	bool m_isInWater = false;
	AABB2 m_bulletShape;
	std::vector <Vertex_PCU> m_bulletVertices;
	Texture* m_bulletTexture = nullptr;
	Vec2 m_fwdDir = Vec2(1.f,  0.f);

};

