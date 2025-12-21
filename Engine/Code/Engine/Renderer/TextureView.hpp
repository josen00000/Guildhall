#pragma once
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"

struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Resource;

union D3d11_Texture_View_Handle_t
{
	ID3D11Resource* m_handle; // top level for release
	ID3D11RenderTargetView*		m_rtv;
	ID3D11ShaderResourceView*	m_srv;
	ID3D11DepthStencilView*		m_dsv;
};

union Texture_View_Handle_t
{
	D3d11_Texture_View_Handle_t d3d11ViewHandle;
	VkImageView vulkanViewHandle;
};

class TextureView {
public:
	TextureView(){}
	~TextureView();

public:
	// Accessor
	ID3D11RenderTargetView*		GetRTVHandle() const { return m_handle.d3d11ViewHandle.m_rtv; }
	ID3D11ShaderResourceView*	GetSRVHandle() const { return m_handle.d3d11ViewHandle.m_srv; }
	ID3D11DepthStencilView*		GetDSVHandle() const { return m_handle.d3d11ViewHandle.m_dsv; }
	VkImageView 				GetVulkanHandle() const { return m_handle.vulkanViewHandle; }

	// Mutator
	void SetRTVHandle( ID3D11RenderTargetView* rtv );
	void SetSRVHandle( ID3D11ShaderResourceView* srv );
	void SetDSVHandle( ID3D11DepthStencilView* dsv );
	void SetVulkanHandle( VkImageView imageView ) { m_handle.vulkanViewHandle = imageView; }

public:
	Texture_View_Handle_t m_handle;
};