#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"





class Explosion :public Entity
{
public:
	~Explosion(){}
	explicit Explosion(Map* map, Vec2 spawnPos);
	virtual void Update( float deltaSeconds ) override;
	virtual void Render() const override;
	virtual void Die() override;
	void CreateSpriteAnimation();

private:
	
public:
	bool m_isInBrick = false;
	float m_currentTime = 0.f;
	float m_duration;
	float m_frameDuration = 0.1f;
	float m_radiu;
	float m_orientationDegrees;
	int m_startSpriteIndex = 0;
	int m_endSpriteIndex = 24;
	Texture* m_texture = nullptr;
	SpriteSheet* m_explosionSprite ;
	SpriteAnimDefinition* m_animation;
	std::vector<Vertex_PCU> m_explosionVertices;
	RandomNumberGenerator m_rng = RandomNumberGenerator();
	
};

