#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"

class RenderContext;
class TextureView;
struct ID3D11Texture2D;


class Texture {
public:
	Texture(){}
	Texture( RenderContext* ctx, ID3D11Texture2D* handle );
	Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle );
	~Texture();
	

	void SetTextureID(const int textureID);
	const int GetTextureID() const {return m_textureID;}
	IntVec2 GetTexelSize() const { return m_texelSizeCoords; }
	TextureView* GetRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();

public:
	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;
	TextureView* m_renderTargetView	= nullptr;
	unsigned int m_textureID	= 0;	
	std::string m_imageFilePath;
	float m_width				= 0;
	float m_height				= 0;
	IntVec2 m_texelSizeCoords;
	TextureView* m_shaderResourcwView = nullptr;
};