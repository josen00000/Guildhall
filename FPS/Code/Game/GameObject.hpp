#pragma once
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/Transform.hpp"

struct Vec3;

class GameObject {
public:
	GameObject();
	~GameObject();

public:
	void SetRotation( Vec3 rot );
	void SetPosition( Vec3 pos );
	void SetScale( Vec3 scale );
	void Render();
public:
	GPUMesh* m_mesh;
	Transform* m_trans;
};