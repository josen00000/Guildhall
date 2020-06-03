#pragma once
#include "Game/Entity.hpp"
#include "engine/Math/AABB2.hpp"


class Texture;


class Player : public Entity
{
public:
	Player();
	~Player();
		
	virtual void Update(float deltaTime) override;
	virtual void Render() const override;
	virtual void Die() override;
	virtual void TakeDamage() override;

private:
	void UpdateTank( float deltaSeconds );
	void UpdateBarrel( float deltaSeconds );
	void RenderTank() const;
	void RenderBarrel() const;
	void CreateTank();

public:
	AABB2 m_tankShape;
	AABB2 m_barrelShape;
	std::vector <Vertex_PCU> m_tankVertices;
	std::vector <Vertex_PCU> m_barrelVertices;
	Texture* m_tankTexture=nullptr;
	Texture* m_barrelTexture=nullptr;
};

