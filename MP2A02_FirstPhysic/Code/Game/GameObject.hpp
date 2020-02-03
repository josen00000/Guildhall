#pragma once
#include "Engine/Core/EngineCommon.hpp"

class Rigidbody2D;
class GameObject {
public:
	GameObject(){}
	~GameObject();

public:
	void Update( float deltaSeconds );
	void Render() const;

	// Mutator
	void SetRigidbody( Rigidbody2D* rb );

private:
	Rigidbody2D* m_rb;
};
