#include "GameObject.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"


extern RenderContext* g_theRenderer;
extern Game* g_theGame;

void GameObject::Update( float deltaSeconds )
{

}

void GameObject::Render() const
{
	Convention gameConvention = g_theGame->GetConvention();
	g_theRenderer->SetModelMatrix( m_trans.ToMatrix( g_theGame->GetConvention() ) );
	g_theRenderer->DrawMesh( m_mesh );
}

void GameObject::SetMesh( GPUMesh* mesh )
{
	m_mesh = mesh;
}

void GameObject::SetPosition( Vec3 pos )
{
	m_trans.SetPosition( pos );
}
