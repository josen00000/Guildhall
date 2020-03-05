#pragma once

class RenderContext;
struct ID3D11SamplerState;

enum SamplerType {
	SAMPLER_POINT,
	SAMPLER_BILINEAR,
};

class Sampler {
public:
	Sampler( RenderContext* ctx, SamplerType type );
	~Sampler();

	inline ID3D11SamplerState* GetHandle() const { return m_handle; } // inline cost memory, speed more fast
public:
	RenderContext* m_owner;
	ID3D11SamplerState* m_handle;
};