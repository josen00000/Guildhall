#include "Sampler.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/Context/RenderContext.hpp"

Sampler::Sampler( RenderContext* ctx, SamplerType type )
{
	m_owner = ctx;
	
	ID3D11Device* device = ctx->GetD3DDevice();
	D3D11_SAMPLER_DESC desc;

	desc.Filter				= ToD3dFilter( type );
	desc.AddressU			= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV			= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW			= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MipLODBias			= 0.0f;
	desc.MaxAnisotropy		= 0;
	desc.ComparisonFunc		= D3D11_COMPARISON_NEVER;
	desc.BorderColor[0]		= 0.0f;
	desc.BorderColor[1]		= 0.0f;
	desc.BorderColor[2]		= 0.0f;
	desc.BorderColor[3]		= 0.0f;
	desc.MinLOD				= 0.0f;
	desc.MaxLOD				= 0.0f;

	device->CreateSamplerState( &desc, &m_handle );
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_handle);
}

D3D11_FILTER ToD3dFilter( SamplerType type )
{
	switch( type )
	{
	case SAMPLER_POINT: return D3D11_FILTER_MIN_MAG_MIP_POINT;
	case SAMPLER_BILINEAR: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	default:
		break;
	}
}
