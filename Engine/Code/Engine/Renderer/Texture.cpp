#include"Texture.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/TextureView.hpp"
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"
#include "Engine/Renderer/RenderContext_d3d11.hpp"
#include "Engine/Renderer/Vulkan/RenderContext_vulkan.hpp"

Texture::Texture( RenderContext* ctx, void* handle, const char* filePath)
	:m_owner(ctx)
{
	if( ctx->GetRenderContextType() == RENDER_CONTEXT_TYPE_D3D11 )
	{
		m_handle.m_d3d11Handle = (ID3D11Texture2D*)handle;
		D3D11_TEXTURE2D_DESC desc;
		m_handle.m_d3d11Handle->GetDesc(&desc);
		m_texelSizeCoords = IntVec2( desc.Width, desc.Height ); 
	}
	else if( ctx->GetRenderContextType() == RENDER_CONTEXT_TYPE_VULKAN )
	{
		m_handle.m_vulkanHandle = (VkImage)handle;
	}

	m_imageFilePath = filePath;
}


Texture::~Texture()
{
	SELF_SAFE_RELEASE(m_renderTargetView);
	SELF_SAFE_RELEASE(m_shaderResourcwView);
	SELF_SAFE_RELEASE(m_depthStencilView);

	m_renderTargetView		= nullptr;
	m_shaderResourcwView	= nullptr;
	m_depthStencilView		= nullptr;

	switch( m_owner->GetRenderContextType() )
	{
		case RENDER_CONTEXT_TYPE_D3D11:
			DX_SAFE_RELEASE(m_handle.m_d3d11Handle);
			break;
		case RENDER_CONTEXT_TYPE_VULKAN:
			ERROR_AND_DIE("not implemented yet");
			break;
		default:
			ERROR_AND_DIE("not implemented yet");
			break;
	}

	// TODO: Need to implement Vulkan cleanup

	m_owner = nullptr;

}

Texture* Texture::CreateDepthStencilBuffer( RenderContext* ctx, int width, int height )
{
	return ctx->CreateDepthStencilBuffer(width, height);
}

TextureView* Texture::GetOrCreateRenderTargetView()
{
	if( m_renderTargetView ){ return m_renderTargetView; }
	RenderContext_d3d11* d3d11Ctx = (RenderContext_d3d11*)m_owner;
	d3d11Ctx->CreateRenderTargetView( this );
	return m_renderTargetView;
}

TextureView* Texture::GetOrCreateShaderResourceView()
{
	if( m_shaderResourcwView ){ return m_shaderResourcwView; }
	RenderContext_d3d11* d3d11Ctx = (RenderContext_d3d11*)m_owner;
	d3d11Ctx->CreateShaderResourceView( this );
	return m_shaderResourcwView;
}

TextureView* Texture::GetDepthStencilView()
{
	return m_depthStencilView;
}

TextureView* Texture::GetOrCreateDepthStencilView()
{
	if( m_depthStencilView ){ return m_depthStencilView; }
	RenderContext_d3d11* d3d11Ctx = (RenderContext_d3d11*)m_owner;
	d3d11Ctx->CreateDepthStencilView( this );
	return m_depthStencilView;
}

TextureView* Texture::GetOrCreateImageView()
{
	if( m_imageView ){ return m_imageView; }
	RenderContext_vulkan* vulkanCtx = (RenderContext_vulkan*)m_owner;
	vulkanCtx->CreateTextureImageView( m_handle.m_vulkanHandle, VK_FORMAT_R8G8B8A8_SRGB );
	return m_imageView;

}

