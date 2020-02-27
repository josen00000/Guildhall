#pragma once
#include "Engine/Math/IntVec2.hpp"
#include <string>

class RenderContext;
class TextureView;
struct ID3D11Texture2D;


class Texture {
public:
	Texture(){}
	~Texture();
	Texture( RenderContext* ctx, ID3D11Texture2D* handle );
	Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle );

	IntVec2 GetTexelSize() const { return m_texelSizeCoords; }
	TextureView* GetRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();

public:
	float m_width				= 0;
	float m_height				= 0;
	IntVec2 m_texelSizeCoords;
	std::string m_imageFilePath;

	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;

	TextureView* m_renderTargetView		= nullptr;
	TextureView* m_shaderResourcwView	= nullptr;
};