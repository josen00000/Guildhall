#include "Game/GameObject.hpp"
#include "Engine/Physics/RigidBody2D.hpp"

GameObject::~GameObject()
{
	m_rb->Destroy();
	m_rb = nullptr;
}

void GameObject::Update( float deltaSeconds )
{
	UNUSED(deltaSeconds);
}

void GameObject::Render() const
{

}

void GameObject::SetRigidbody( Rigidbody2D* rb )
{
	m_rb = rb;
}
