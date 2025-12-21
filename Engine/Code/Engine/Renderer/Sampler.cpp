#include "Sampler.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"

Sampler::Sampler( RenderContext* ctx, SamplerType type )
{
	m_owner = ctx;
	m_handle = nullptr;
	
	ID3D11Device* dev = ctx->m_device;
	D3D11_SAMPLER_DESC desc;

	if( type == SAMPLER_POINT ) {
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	}
	else if( type == SAMPLER_BILINEAR ) {
		desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	desc.MipLODBias = 0.0f;
	desc.MaxAnisotropy = 0;;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.BorderColor[0] = 0.0f;
	desc.BorderColor[1] = 0.0f;
	desc.BorderColor[2] = 0.0f;
	desc.BorderColor[3] = 0.0f;
	desc.MinLOD = 0.0f;
	desc.MaxLOD = 0.0f;


	dev->CreateSamplerState( &desc, &m_handle );
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_handle);
}
