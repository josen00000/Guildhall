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
	delete m_depthStencilView;

	m_renderTargetView		= nullptr;
	m_shaderResourcwView	= nullptr;
	m_depthStencilView		= nullptr;
	m_owner = nullptr;
	DX_SAFE_RELEASE(m_handle);

}

Texture* Texture::CreateDepthStencilBuffer( RenderContext* ctx, int width, int height )
{
	// Create depth texture desc
	D3D11_TEXTURE2D_DESC desc;
	desc.Width				= width;
	desc.Height				= height;
	desc.MipLevels			= 1;
	desc.ArraySize			= 1;
	desc.Format				= DXGI_FORMAT_D32_FLOAT;
	desc.SampleDesc.Count	= 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage				= D3D11_USAGE_DEFAULT; // mip-chains, GPU/DEFAUTE
	desc.BindFlags			= D3D11_BIND_DEPTH_STENCIL; //| D3D11_BIND_SHADER_RESOURCE do it later only be r 32; // what does it means?
	desc.CPUAccessFlags		= 0;
	desc.MiscFlags			= 0;
	
	ID3D11Texture2D* texHandle = nullptr;
	ctx->m_device->CreateTexture2D( &desc, NULL, &texHandle );

	Texture* depthBuffer = new Texture( ctx, texHandle );
	return depthBuffer;
}

void Texture::SetTextureID( int textureID )
{
	m_textureID=textureID;
}

TextureView* Texture::GetOrCreateRenderTargetView()
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

TextureView* Texture::GetOrCreateDepthStencilView()
{
	if( m_depthStencilView != nullptr ){ return m_depthStencilView; }

	ID3D11Device* dev = m_owner->m_device;
	ID3D11DepthStencilView* dsv = nullptr;
	dev->CreateDepthStencilView( m_handle, nullptr, &dsv );

	if( dsv != nullptr ){
		m_depthStencilView = new TextureView();
		m_depthStencilView->m_dsv = dsv;
	}
	return m_depthStencilView;
}

