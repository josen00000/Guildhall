#include"Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/TextureView.hpp"


Texture::Texture( RenderContext* ctx, ID3D11Texture2D* handle )
	:m_owner(ctx)
	,m_handle(handle)
{
	D3D11_TEXTURE2D_DESC desc;
	handle->GetDesc(&desc);
	m_texelSizeCoords = IntVec2( desc.Width, desc.Height ); 
}

Texture::Texture( const char* filePath, RenderContext* ctx, ID3D11Texture2D* handle )
{
	m_owner = ctx;
	m_handle = handle;
	m_imageFilePath = filePath;

	D3D11_TEXTURE2D_DESC desc;
	m_handle->GetDesc( & desc );
	m_texelSizeCoords = IntVec2( desc.Width, desc.Height );
}

Texture::~Texture()
{
	delete m_renderTargetView;
	delete m_shaderResourcwView;
	m_renderTargetView = nullptr;
	m_shaderResourcwView = nullptr;
	m_owner = nullptr;
	DX_SAFE_RELEASE(m_handle);

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

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if( m_shaderResourcwView != nullptr ){ return m_shaderResourcwView; }

	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;
	dev->CreateShaderResourceView( m_handle, nullptr, &srv );
	if( srv!= nullptr ) {
		m_shaderResourcwView = new TextureView();
		m_shaderResourcwView->m_srv = srv;
	}
	return m_shaderResourcwView;
}
