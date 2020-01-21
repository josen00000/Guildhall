#pragma once
#include <string>
#include <queue>
#include "Engine/Core/Rgba8.hpp"

class RenderContext;
class Camera;
class BitmapFont;
struct Rgba8;
struct Vertex_PCU;



struct ColoredLine {

	ColoredLine( Rgba8 c, std::string t)
		:color(c)
		,text(t)
	{
	};
	Rgba8 color;
	std::string text;
};


class DevConsole
{


public:
	explicit DevConsole( BitmapFont* font);
	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	void PrintString( const Rgba8& textColor, const std::string& devConsolePrintString);
	void SetIsOpen( bool isOpen);
	bool IsOpen() const;
	void Render( RenderContext& renderer, const Camera& camera, float lineHeight) const;

private:

public:
	//std::vector<Vertex_PCU> vertices;
	std::vector<ColoredLine> m_lines;
	BitmapFont* m_font = nullptr;
	bool m_isOpen = false;
};

