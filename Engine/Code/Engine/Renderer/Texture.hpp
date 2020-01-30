#pragma once
#include "Engine/Math/IntVec2.hpp"

class RenderContext;
class TextureView;
struct ID3D11Texture2D;


class Texture {
public:
	Texture(){}
	Texture( RenderContext* ctx, ID3D11Texture2D* handle );
	Texture(const unsigned int textureID,const char* imageFilePath, const float width, const float height);
	~Texture();
	

	void SetTextureID(const int textureID);
	const int GetTextureID() const {return m_textureID;}
	IntVec2 GetTexelSize() const { return m_texelSizeCoords; }
	TextureView* GetRenderTargetView();

public:
	RenderContext* m_owner		= nullptr;
	ID3D11Texture2D* m_handle	= nullptr;
	TextureView* m_renderTargetView	= nullptr;
	unsigned int m_textureID	= 0;	
	const char* m_imageFilePath	=nullptr;
	float m_width				= 0;
	float m_height				= 0;
	IntVec2 m_texelSizeCoords;
};