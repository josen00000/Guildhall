#pragma once
#include "Engine/Core/EngineCommon.hpp"

class RenderContext;
struct ID3D11Buffer;

#define BIT_FLAG(b)		(1 << (b))

// what we use it for
// naming. bit shows only one
enum RenderBufferUsageBit : uint {
	VERTEX_BUFFER_BIT		= BIT_FLAG(0),
	INDEX_BUFFER_BIT		= BIT_FLAG(1),
	UNIFORM_BUFFER_BIT		= BIT_FLAG(2)
};
typedef uint RenderBufferUsage;

// How are we going to access it (memory)
enum RenderMemoryHint : uint {
	MEMORY_HINT_GPU,		// GPU can read/write, cpu can't touch it, changes rarely from CPU
	MEMORY_HINT_DYNAMIC,	// GPU memory(read/write), often change from CPU, allows to map the memory
	MEMORY_HINT_STAGING		// CPU read/write, and can copy from cpu-gpu
};

class RenderBuffer {
public:
	RenderBuffer( RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint );
	~RenderBuffer();

	bool Update( void const* data, size_t dataByteSize, size_t elementByteSize );
	bool IsCompatible( size_t dataByteSize, size_t elementByteSize );
	void Cleanup();
private:
	bool Create( size_t dataByteSize, size_t elementByteSize );

public:
		RenderContext* m_owner	= nullptr;
		ID3D11Buffer* m_handle	= nullptr;

		RenderBufferUsage m_usage;
		RenderMemoryHint m_memHint;

		 size_t m_bufferByteSize;
		 size_t m_elementByteSize;
};


class VertexBuffer : public RenderBuffer {
public:
	VertexBuffer( RenderContext* ctx, RenderMemoryHint hint ) 
		:RenderBuffer( ctx, VERTEX_BUFFER_BIT, hint )
	{

	}

};