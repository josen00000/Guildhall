#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/GPU/RenderBuffer.hpp"
class RenderContext;
struct ID3D11Buffer;

class VertexBuffer : public RenderBuffer {
public:
	VertexBuffer( RenderContext* ctx, RenderMemoryHint hint );

public:
	const buffer_attribute_t* GetAttribute() const { return m_attributes; }
	void UpdateVertices( int vCount, const void* vertices, buffer_attribute_t* attribute );
	void SetLayout( const buffer_attribute_t* attributes );
public:
	buffer_attribute_t const* m_attributes; // array describing the vertex
};