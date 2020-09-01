#include "UIButton.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/RenderContext.hpp"

extern RenderContext*	g_theRenderer;
extern BitmapFont*		g_squirrelFont;
extern EventSystem*		g_theEventSystem;

Rgba8 UIButton::s_selectTextColor = Rgba8::YELLOW;

UIButton::UIButton( Vec2 spawnPos )
	:m_pos(spawnPos)
{
	m_shape.SetCenter( m_pos );
}

UIButton* UIButton::CreateButtonWithPosSizeAndText( Vec2 pos, Vec2 size, std::string text )
{
	UIButton* result = new UIButton( pos );
	result->m_shape.SetDimensions( size );
	result->SetDisplayText( text );
	return result;
}

void UIButton::SetPosition( Vec2 pos )
{
	m_pos = pos;
	m_shape.SetCenter( m_pos );
}

void UIButton::SetDimension( Vec2 dimension )
{
	m_dimension = dimension;
	m_shape.SetDimensions( dimension );
}

void UIButton::SetDisplayText( std::string displayText )
{
	m_text = displayText;
}

void UIButton::SetEventName( std::string eventName )
{
	m_eventName = eventName;
}

void UIButton::SetTintColor( Rgba8 color )
{
	m_tintColor = color;
}

void UIButton::SetTextColor( Rgba8 color )
{
	m_textColor = color;
}

void UIButton::SetSelectColor( Rgba8 color )
{
	m_SelectColor = color;
}

void UIButton::SetIsSelecting( bool isSelecting )
{
	m_isSelecting = isSelecting;
}

void UIButton::ExecuteTargetFunction()
{
	g_theEventSystem->FireEvent( m_eventName, m_eventArgs );
}

void UIButton::RenderButton()
{
	static int debugTimer = 0;
	debugTimer++;
	g_theRenderer->DrawAABB2D( m_shape, m_tintColor );
	//DebugAddScreenText( Vec4( m_pos.x, m_pos.y, 0.f, 0.f ), Vec2( 0.5f ), 1.f, m_textColor, m_textColor, 0.f, m_text );
	if( m_isSelecting ) {
		
	}
	Texture* textTexture = g_squirrelFont->GetTexture();
	g_theRenderer->SetDiffuseTexture( textTexture );
	g_theRenderer->DrawVertexVector( m_textVerts );
	g_theRenderer->SetDiffuseTexture( nullptr );
}

void UIButton::UpdateButton()
{
	m_textVerts.clear();
	Rgba8 tempColor;
	if( m_isSelecting ) {
		tempColor = m_SelectColor;
	}
	else {
		tempColor = m_textColor;
	}
	g_squirrelFont->AddVertsForTextInBox2D( m_textVerts, m_shape, 7.f, m_text, tempColor, 1.f );
}

