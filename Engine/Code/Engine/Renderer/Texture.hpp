#pragma once
#include "Engine/Math/IntVec2.hpp"
//#include "C:\VulkanSDK\1.4.309.0\Include\vulkan\vulkan.h"
#include <string>

class RenderContext;
class renderContext_d3d11;
class RenderContext_vulkan;
class TextureView;
struct ID3D11Texture2D;
typedef struct VkImage_T* VkImage;


union Texture_handle_t
{
	ID3D11Texture2D* m_d3d11Handle;
	VkImage m_vulkanHandle;
};

class Texture {
public:
	friend class RenderContext_d3d11;
	friend class RenderContext_vulkan;

	Texture(){}
	~Texture();
	Texture( RenderContext* ctx, void* handle, const char* filePath = "" );
	static Texture* CreateDepthStencilBuffer( RenderContext* ctx, int width, int height );


	// accessor
	ID3D11Texture2D* GetD3D11Handle() const { return m_handle.m_d3d11Handle; }
	VkImage GetVulkanHandle() const { return m_handle.m_vulkanHandle; }
	int		GetHeight() const { return m_texelSizeCoords.y; }
	int		GetWidth() const { return m_texelSizeCoords.x; }
	IntVec2	GetSize() const { return m_texelSizeCoords; }

	TextureView* GetOrCreateRenderTargetView();
	TextureView* GetOrCreateShaderResourceView();
	TextureView* GetDepthStencilView();
	TextureView* GetOrCreateDepthStencilView();
	TextureView* GetOrCreateImageView();

public:
	IntVec2 m_texelSizeCoords;
	std::string m_imageFilePath;

	TextureView* m_renderTargetView		= nullptr;	// for d3d11 usage
	TextureView* m_shaderResourcwView	= nullptr;	// for d3d11 usage
	TextureView* m_depthStencilView		= nullptr;	// for d3d11 usage
	TextureView* m_imageView			= nullptr;	// for vulkan usage

private:
	RenderContext* m_owner		= nullptr;
	Texture_handle_t m_handle;

};