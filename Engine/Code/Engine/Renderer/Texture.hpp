#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"

class RenderContext;
class TextureView;
struct ID3D11Texture2D;

class Texture {
public:
	Texture(){}
	~Texture();
	Texture( RenderContext* ctx, ID3D11Texture2D* handle );
	Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle );
	static Texture* CreateDepthStencilBuffer( RenderContext* ctx, int width, int height );


	// accessor
	ID3D11Texture2D* GetHandle() const { return m_handle; }
	int		GetHeight() const { return m_texelSizeCoords.y; }
	int		GetWidth() const { return m_texelSizeCoords.x; }
	IntVec2	GetSize() const { return m_texelSizeCoords; }

	TextureView* GetOrCreateRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	TextureView* GetDepthStencilView();
	TextureView* GetOrCreateDepthStencilView();

public:
	IntVec2 m_texelSizeCoords;
	std::string m_imageFilePath;

	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;

	TextureView* m_renderTargetView		= nullptr;
	TextureView* m_shaderResourcwView	= nullptr;
	TextureView* m_depthStencilView		= nullptr;
};