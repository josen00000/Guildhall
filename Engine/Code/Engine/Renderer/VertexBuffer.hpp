#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
class RenderContext;
struct ID3D11Buffer;

class VertexBuffer : public RenderBuffer {
public:
	VertexBuffer( RenderContext* ctx, RenderMemoryHint hint );
	void UpdateVertices( int vCount, const void* vertices, buffer_attribute_t* attribute );
	void SetLayout( const buffer_attribute_t* attributes );
public:
	buffer_attribute_t const* m_attributes; // array describing the vertex
};