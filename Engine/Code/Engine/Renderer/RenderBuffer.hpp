#pragma once
#include "Engine/Core/EngineCommon.hpp"

class RenderContext;
struct ID3D11Buffer;
class RenderContext_d3d11;
class RenderContext_vulkan;

// what we use it for
// naming. bit shows only one
enum RenderBufferUsageBit : uint {
	VERTEX_BUFFER_BIT		= BIT_FLAG(0),
	INDEX_BUFFER_BIT		= BIT_FLAG(1),
	UNIFORM_BUFFER_BIT		= BIT_FLAG(2),
};
typedef uint RenderBufferUsage;

// How are we going to access it (memory)
enum RenderMemoryHint : uint {
	MEMORY_HINT_GPU,		// GPU can read/write, cpu can't touch it, changes rarely from CPU
	MEMORY_HINT_DYNAMIC,	// GPU memory(read/write), often change from CPU, allows to map the memory
	MEMORY_HINT_STAGING		// CPU read/write, and can copy from cpu-gpu
};

class RenderBuffer {
	friend class RenderContext_d3d11;
	friend class RenderContext_vulkan;

public:
	RenderBuffer( char const* debugName, RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint );
	~RenderBuffer();

	void Update( void const* data, size_t dataByteSize, size_t elementByteSize );
	bool IsCompatible( size_t dataByteSize, size_t elementByteSize );
	void Cleanup();
	int GetMemoryUsage();

private:
	int GetVulkanMemoryUsage();
	int GetDXMemoryUsage();
	unsigned int GetDXUsage();
	int GetVulkanUsage();

public:

	RenderBufferUsage m_usage;
	RenderMemoryHint m_memHint;

	size_t m_bufferByteSize;
	size_t m_elementByteSize;
	std::string m_debugName;

private:
	RenderContext* m_owner	= nullptr;
	void* m_handle	= nullptr; // buffer handle for specific render context
	void* m_mappedMemory = nullptr; // mapped memory for specific render context. Only used for Vulkan for now.
	void* m_mappedMemoryData = nullptr; // mapped data for specific render context. Only used for Vulkan for now.

};


