#include "RenderBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/GPU/RenderBuffer.hpp"
#include "Engine/Renderer/Context/RenderContext.hpp"


RenderBuffer::RenderBuffer( RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint )	
	:m_owner( owner ) // Run order how it declare, in memory order
	,m_usage( usage )
	,m_memHint( memHint )
{
	m_handle = nullptr;
	m_dataByteSize = 0U;
	m_elementByteSize = 0U;
}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

bool RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementByteSize )
{
	if( !IsCompatible( dataByteSize, elementByteSize ) ) {
		Cleanup();
		CreateBuffer( dataByteSize, elementByteSize );
	}

	ID3D11DeviceContext* ctx = m_owner->GetD3DContext();
	
	if( m_memHint == MEMORY_HINT_DYNAMIC ) {
		D3D11_MAPPED_SUBRESOURCE mapped;
		HRESULT result = ctx->Map( m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped );
		if( SUCCEEDED(result) ){
			memcpy( mapped.pData, data, dataByteSize );
			ctx->Unmap( m_handle, 0 );
		}
		else {
			return false;
		}
	}
	else {
		ctx->UpdateSubresource( m_handle, 0, nullptr, data, 0, 0 );
	}
	return true;
}

bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize )
{
	// 1 have handle
	// same element byte size
	// compare data byte size
	if( nullptr == m_handle ){ return false; }
	if( m_elementByteSize != elementByteSize ) { return false; }
	if( m_memHint = MEMORY_HINT_DYNAMIC ){
		return dataByteSize <= elementByteSize;
	}

	return dataByteSize == elementByteSize;

}

void RenderBuffer::Cleanup()
{
	DX_SAFE_RELEASE(m_handle);
	m_dataByteSize		= 0;
	m_elementByteSize	= 0;
}

D3D11_USAGE ToDXMemoryUsage( RenderMemoryHint hint )
{
	switch( hint )
	{
	case MEMORY_HINT_GPU:		return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC:	return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING:	return D3D11_USAGE_STAGING;
	default:					ERROR_AND_DIE( "Unknown hint!" );
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

bool RenderBuffer::CreateBuffer( size_t dataByteSize, size_t elementByteSize )
{
	ID3D11Device* device = m_owner->GetD3DDevice();
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = ToDXMemoryUsage( m_memHint );
	desc.BindFlags = ToDXUsage( m_usage );
	desc.CPUAccessFlags = 0;
	if( m_memHint == MEMORY_HINT_DYNAMIC ) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if( m_memHint == MEMORY_HINT_STAGING ) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	desc.MiscFlags = 0;
	desc.StructureByteStride = (UINT)elementByteSize;

	device->CreateBuffer( &desc, nullptr, &m_handle );

	m_dataByteSize = dataByteSize;
	m_elementByteSize = elementByteSize;

	if( nullptr != m_handle ) {
		//m_handle->SetPrivateData( WKPDID_D3DDebugObjectName, (size_t)m_debugName.size(), m_debugName.c_str() ); // debug for leaking
	}
	
	return nullptr != m_handle;
}
