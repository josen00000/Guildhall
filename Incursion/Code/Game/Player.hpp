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
	void UpdateTank( float deltaSeconds );
	void UpdateXboxVibration(float deltaSeconds);
	void UpdateBarrel( float deltaSeconds );
	void RenderTank() const;
	void RenderBarrel() const;
	void CreateTank();
	void Fire();

public:
	AABB2 m_tankShape;
	AABB2 m_barrelShape;
	std::vector <Vertex_PCU> m_tankVertices;
	std::vector <Vertex_PCU> m_barrelVertices;
	Texture* m_tankTexture=nullptr;
	Texture* m_barrelTexture=nullptr;
	SoundID playerShoot = (size_t)-1;
	SoundID playerHit = (size_t)-1;
	SoundID playerDie = (size_t)-1;
	float m_vibrationIntense = 0.f;
	float m_vibrationDownRate = 0.5f;
	float m_barrelOrientationDegrees = 0.f;
	float m_barrelAngularVelocity = BARREL_ANGULAR_VELOCITY;
	float m_deltaDegrees = 0.f;
	float m_timeInLava = 0.f;
};

