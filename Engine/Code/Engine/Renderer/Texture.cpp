#include"Texture.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"


Texture::Texture(const unsigned int textureID, const char* imageFilePath, const float width, const float height )
	:m_textureID(textureID)
	,m_imageFilePath(imageFilePath)
	,m_width(width)
	,m_height(height)
{

}

Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle )
	:m_owner(ctx)
	,m_handle(handle)
{

}

Texture::~Texture()
{
	delete m_renderTargetView;
	m_renderTargetView = nullptr;
	m_owner = nullptr;
	DX_SAFE_RELEASE(m_handle);
}

void Texture::SetTextureID( int textureID )
{
	m_textureID=textureID;
}

TextureView* Texture::GetRenderTargetView()
{
	if( m_renderTargetView ){
		return m_renderTargetView;
	}

	ID3D11Device* dev = m_owner->m_device;
	ID3D11RenderTargetView* rtv = nullptr;
	dev->CreateRenderTargetView( m_handle, nullptr, &rtv );
	if( rtv != nullptr ){
		m_renderTargetView = new TextureView();
		m_renderTargetView->SetRTVHandle( rtv );
	}

	
	return m_renderTargetView;
}
