#pragma once
#include "Engine/Renderer/RenderContext.hpp"

class RenderContext;

RenderContext* CreateOrGetRenderContext( RenderContextType type );

// Font
BitmapFont* CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath );
Texture* CreateOrGetTextureFromFile( const char* imageFilePath );
Texture* CreateTextureFromColor( Rgba8 color );
Texture* CreateTextureFromVec4( Vec4 input );
