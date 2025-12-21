#include "Sampler.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Sampler::Sampler( RenderContext* ctx, SamplerType type )
{
	m_owner = ctx;
	m_handle.d3d11Handle = nullptr;
	m_type = type;
	ctx->CreateTextureSampler( this );
}

Sampler::~Sampler()
{
	switch( m_owner->GetRenderContextType() )
	{
		case RENDER_CONTEXT_TYPE_D3D11:
			DX_SAFE_RELEASE(m_handle.d3d11Handle);
			break;
		case RENDER_CONTEXT_TYPE_VULKAN:
			ERROR_RECOVERABLE("Vulkan Sampler not implemented");
			break;
		default:
			break;
	}
}
