#include "GameObject.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Transform.hpp"

extern RenderContext* g_theRenderer;

GameObject::GameObject()
{
	m_trans = new Transform();
}

GameObject::~GameObject()
{
	delete m_trans;
	m_trans	= nullptr;
	m_mesh	= nullptr;
}

void GameObject::SetRotation( Vec3 rot )
{
	m_trans->SetPitchDegrees( rot.x );
	m_trans->SetRollDegrees( rot.y );
	m_trans->SetYawDegrees( rot.z );
	
	//SetRotationFromPitchRollYawDegrees( rot );
}

void GameObject::SetPosition( Vec3 pos )
{
	m_trans->SetPosition( pos );
}

void GameObject::SetScale( Vec3 scale )
{
	m_trans->SetScale( scale );
}

void GameObject::Render()
{
	if( m_tex != nullptr ) {
		g_theRenderer->SetDiffuseTexture( m_tex );
	}
	g_theRenderer->SetModelMatrix( m_trans->ToMatrix() );
	g_theRenderer->DrawMesh( m_mesh );
}
