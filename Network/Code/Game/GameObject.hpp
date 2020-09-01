#pragma once
#include "Engine/Core/Transform.hpp"

class GPUMesh;
class Game;

class GameObject {
public:
	GameObject(){}
	~GameObject(){}
	void Update( float deltaSeconds );
	void Render() const;

	GPUMesh* GetMesh() const { return m_mesh; }
	void SetMesh( GPUMesh* mesh );
	void SetPosition( Vec3 pos );

private:
	Game* m_owner = nullptr;
	Transform m_trans;
	GPUMesh* m_mesh = nullptr;
};