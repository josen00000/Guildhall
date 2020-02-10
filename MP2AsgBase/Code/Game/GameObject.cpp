#include "GameObject.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

GameObject::~GameObject()
{
	m_rb->Destroy();
	m_rb = nullptr;
}

void GameObject::Update( float deltaSeconds )
{

}

void GameObject::Render() const
{

}
