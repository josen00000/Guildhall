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

	void SetTextureID( const int textureID );
	const int GetTextureID() const { return m_textureID; }
	IntVec2 GetTexelSize() const { return m_texelSizeCoords; }
	TextureView* GetOrCreateRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	TextureView* GetDepthStencilView();
	TextureView* GetOrCreateDepthStencilView();

public:
	float m_width				= 0;
	float m_height				= 0;
	unsigned int m_textureID	= 0;	
	IntVec2 m_texelSizeCoords;
	std::string m_imageFilePath;

	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;

	TextureView* m_renderTargetView		= nullptr;
	TextureView* m_shaderResourcwView	= nullptr;
	TextureView* m_depthStencilView		= nullptr;
};