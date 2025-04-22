#pragma once

#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"

class RenderContext;
struct ID3D11SamplerState;
class RenderContext_d3d11;
class RenderContext_vulkan;

enum SamplerType {
	SAMPLER_POINT,
	SAMPLER_BILINEAR,
};

union SamplerHandle{
	ID3D11SamplerState* d3d11Handle;
	VkSampler vulkanHandle;
};
class Sampler {
	friend class RenderContext_d3d11;
	friend class RenderContext_vulkan;

public:
	Sampler( RenderContext* ctx, SamplerType type );
	~Sampler();

	ID3D11SamplerState* GetD3D11Handle() const { return m_handle.d3d11Handle; }
	VkSampler GetVulkanHandle() const { return m_handle.vulkanHandle; }

private:
	RenderContext* m_owner;
	SamplerHandle m_handle;
	SamplerType m_type;
};