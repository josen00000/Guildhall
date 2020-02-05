#pragma once
#include "Engine/Core/EngineCommon.hpp"

struct Vec2;

class Rigidbody2D;
class GameObject {
public:
	GameObject( Vec2 pos, float radius );
	~GameObject();

public:
	void Update( float deltaSeconds );
	void Render() const;

	// Mutator
	void SetRigidbody( Rigidbody2D* rb );

private:
	bool m_isDestroied = false;
	Vec2 m_pos;
	float m_radius;
	Rigidbody2D* m_rb;
};
