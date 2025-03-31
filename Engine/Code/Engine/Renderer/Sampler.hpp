#pragma once

class RenderContext_d3d11;
struct ID3D11SamplerState;

enum SamplerType {
	SAMPLER_POINT,
	SAMPLER_BILINEAR,
};

class Sampler {
public:
	Sampler( RenderContext_d3d11* ctx, SamplerType type );
	~Sampler();

	inline ID3D11SamplerState* GetHandle() const { return m_handle; } // inline cost memory, speed more fast
public:
	RenderContext_d3d11* m_owner;
	ID3D11SamplerState* m_handle;
};