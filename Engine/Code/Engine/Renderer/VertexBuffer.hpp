#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
class RenderContext;
struct ID3D11Buffer;

class VertexBuffer : public RenderBuffer {
public:
	VertexBuffer( RenderContext* ctx, RenderMemoryHint hint );
		

public:
	size_t m_stride; // how far from one vertex to the next
	//buffer_attribute_t const* m_attributes; // array describing the vertex
};