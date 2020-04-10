#include "RenderBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"

RenderBuffer::RenderBuffer( RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint )
	:m_owner(owner) // Run order how it declare, in memory order
	,m_usage(usage)
	,m_memHint(memHint)
{
	m_handle = nullptr;
	m_bufferByteSize = 0U;
	m_elementByteSize = 0U;
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

bool RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementByteSize )
{
	// 1. if not compatible - destroy the old buffer
	if( !IsCompatible( dataByteSize,elementByteSize ) ) {
		Cleanup();// destroy the handle, reset things
		Create( dataByteSize, elementByteSize );
	}
	// our elementSize matches the passed in 
	// if we're GPU
		// bufferSizes MUST match
	// if we're dynamic
		// passed in buffer size is less than our bufferSize 
	// 2. if no buffer, create one that is compatible
	// 3. updating the buffer

	ID3D11DeviceContext* ctx = m_owner->m_context;
	if( m_memHint == MEMORY_HINT_DYNAMIC ){
		// mapping
		// memcpy( very fast for bit to bit copy )
		// block call : wait for the result 
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT  result = ctx->Map( m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped ); // lock the memory
		if( SUCCEEDED( result ) ) {
			memcpy( mapped.pData, data, dataByteSize );
			ctx->Unmap( m_handle, 0 );
		}
		else {
			return false;
		}
	}
	else {
		// if this is MEMORY_HINT_GPU (gpu memory)
		ctx->UpdateSubresource( m_handle, 0, nullptr, data, 0, 0 );
	}
	return true;
		// mapping buffer
	// Only available to DYNAMIC buffer,
	// but, don't have to reallocate if going smaller

	// CopySubresource ( direct copy )
	//	Only available to GPU buffers that have 
	//	exactly the same size, and element size
}

bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize )
{
	if( m_handle == nullptr ) {
		return false;

	}

	if( m_elementByteSize != elementByteSize ) {
		return false;
	}

	if( m_memHint == MEMORY_HINT_DYNAMIC ) {
		return dataByteSize <= m_bufferByteSize;
	}
	else {
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::Cleanup()
{
	DX_SAFE_RELEASE(m_handle);
	m_bufferByteSize	= 0;
	m_elementByteSize	= 0;
}

D3D11_USAGE ToDXMemoryUsage( RenderMemoryHint hint )
{
	switch( hint )
	{
	case MEMORY_HINT_GPU:		return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC:	return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING:	return D3D11_USAGE_STAGING;
	default:					ERROR_AND_DIE( "Unknown hint! " );
	//return D3D11_USAGE_DYNAMIC;
	}
}

UINT ToDXUsage( RenderBufferUsage usage )
{
	UINT ret = 0;
	if( usage & VERTEX_BUFFER_BIT ) {
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if( usage & INDEX_BUFFER_BIT ) {
		ret |= D3D11_BIND_INDEX_BUFFER;
	}
	if( usage & UNIFORM_BUFFER_BIT ) {
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

bool RenderBuffer::Create( size_t dataByteSize, size_t elementByteSize )
{
	ID3D11Device* device = m_owner->m_device;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );
	desc.CPUAccessFlags = 0;
	if( m_memHint == MEMORY_HINT_DYNAMIC ) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if ( m_memHint == MEMORY_HINT_STAGING){
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	desc.MiscFlags = 0;
	desc.StructureByteStride = (UINT)elementByteSize;
	device->CreateBuffer( &desc, nullptr, &m_handle );

	m_bufferByteSize = dataByteSize;
	m_elementByteSize = elementByteSize;
	return ( m_handle != nullptr );
}
