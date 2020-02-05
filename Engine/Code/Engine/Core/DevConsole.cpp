#include "DevConsole.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/BitmapFont.hpp"

extern RenderContext* g_theRenderer;

DevConsole::DevConsole( BitmapFont* font )
	:m_font(font)
{

}

void DevConsole::Startup()
{

}

void DevConsole::BeginFrame()
{

}

void DevConsole::EndFrame()
{

}

void DevConsole::Shutdown()
{

}


void DevConsole::PrintString( const Rgba8& textColor, const std::string& devConsolePrintString )
{
	ColoredLine tem = ColoredLine(textColor, devConsolePrintString);
	m_lines.push_back(tem);
}

void DevConsole::Render( RenderContext& renderer, const Camera& camera, float lineHeight ) const
{
	if(!m_isOpen){ return;}
	std::vector<Vertex_PCU> vertices;
	int maxDisplayedLinesNum = (int)(camera.GetHeight() / lineHeight);
	int index = (int)(m_lines.size() - 1);
	Vec2 minsPos = camera.GetOrthoBottomLeft();
	if(index == 0){ return;}
	while(index >= m_lines.size() - 1 - maxDisplayedLinesNum){
		ColoredLine tem = m_lines[index];
		m_font->AddVertsForText2D(vertices, minsPos, lineHeight, tem.text, tem.color);
		minsPos.y += lineHeight;
		index--;
		
	}
	renderer.DrawVertexVector(vertices);
}



void DevConsole::SetIsOpen( bool isOpen )
{
	m_isOpen = isOpen;
}

bool DevConsole::IsOpen() const
{
	return m_isOpen;
}

