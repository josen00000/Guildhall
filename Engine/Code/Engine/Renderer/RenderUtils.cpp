#pragma once

#include "RenderUtils.hpp"
#include "Engine/Renderer/RenderContext_d3d11.hpp"
#include "Engine/Renderer/RenderContext_vulkan.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

RenderContext* CreateOrGetRenderContext( RenderContextType type )
{
	if( RenderContext::s_renderContext )
	{
		return RenderContext::s_renderContext;
	}

	switch( type )
	{
		case RENDER_CONTEXT_TYPE_D3D11:
			return new RenderContext_d3d11();
		case RENDER_CONTEXT_TYPE_OPENGL:
			ERROR_AND_DIE( "OpenGL not supported" );
		case RENDER_CONTEXT_TYPE_VULKAN:
			return new RenderContext_vulkan();
		case RENDER_CONTEXT_TYPE_NONE:
			return nullptr;
	}

}

BitmapFont* CreateOrGetBitmapFontFromFile( const char* fontName, const char* fontFilePath )
{
	return nullptr;
}

Texture* CreateOrGetTextureFromFile( const char* imageFilePath )
{
	return nullptr;
}

Texture* CreateTextureFromColor( Rgba8 color )
{
	return nullptr;
}

Texture* CreateTextureFromVec4( Vec4 input )
{
	return nullptr;
}
