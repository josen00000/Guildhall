#include "RenderBuffer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/Vulkan/VulkanCommon.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext_d3d11.hpp"


RenderBuffer::RenderBuffer( char const* debugName, RenderContext* owner, RenderBufferUsage usage, RenderMemoryHint memHint )	
	:m_owner( owner ) // Run order how it declare, in memory order
	,m_usage( usage )
	,m_memHint( memHint )
	,m_debugName(debugName)
{
	m_handle = nullptr;
	m_bufferByteSize = 0U;
	m_elementByteSize = 0U;
}

RenderBuffer::~RenderBuffer()
{
	switch( m_owner->GetRenderContextType() )
	{
		case RENDER_CONTEXT_TYPE_D3D11:
			ID3D11Buffer* buffer = (ID3D11Buffer*)m_handle;
			DX_SAFE_RELEASE(buffer);
			break;
	}
}

bool RenderBuffer::IsCompatible( size_t dataByteSize, size_t elementByteSize )
{
	// if we're GPU
		// bufferSizes MUST match
	// if we're dynamic
		// passed in buffer size is less than our bufferSize 
	if( m_handle == nullptr ) {
		return false;

	}

	if( m_elementByteSize != elementByteSize ) {
		return false;
	}

	if( m_memHint == MEMORY_HINT_DYNAMIC ) 
	{
		return dataByteSize <= m_bufferByteSize;
	}
	else 
	{
		return dataByteSize == m_bufferByteSize;
	}
}

void RenderBuffer::Cleanup()
{
	m_owner->CleanUpRenderBuffer( *this );
	m_bufferByteSize	= 0;
	m_elementByteSize	= 0;
}

int RenderBuffer::GetDXMemoryUsage( )
{
	switch( m_memHint )
	{
	case MEMORY_HINT_GPU:		return D3D11_USAGE_DEFAULT;
	case MEMORY_HINT_DYNAMIC:	return D3D11_USAGE_DYNAMIC;
	case MEMORY_HINT_STAGING:	return D3D11_USAGE_STAGING;
	default:					ERROR_AND_DIE( "Unknown hint! " );
	//return D3D11_USAGE_DYNAMIC;
	}
}


unsigned int RenderBuffer::GetDXUsage( )
{
	UINT ret = 0;
	if( m_usage & VERTEX_BUFFER_BIT ) {
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if( m_usage & INDEX_BUFFER_BIT ) {
		ret |= D3D11_BIND_INDEX_BUFFER;
	}
	if( m_usage & UNIFORM_BUFFER_BIT ) {
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

int RenderBuffer::GetVulkanUsage()
{
	int ret = 0;
	if( m_usage & VERTEX_BUFFER_BIT ) {
		ret |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	}
	if( m_usage & INDEX_BUFFER_BIT ) {
		ret |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	}
	if( m_usage & UNIFORM_BUFFER_BIT ) {
		ret |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	}

	if( m_memHint == MEMORY_HINT_GPU ){
		ret |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	}

	return ret;
}

/*
bool RenderBuffer::D3d11Create( size_t dataByteSize, size_t elementByteSize )
{
	RenderContext_d3d11* ctx = dynamic_cast<RenderContext_d3d11*>( m_owner );
	ID3D11Device* device = ctx->m_device;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = (UINT)dataByteSize;
	desc.Usage = GetDXMemoryUsage( m_memHint );
	desc.BindFlags = GetDXUsage( m_usage );
	desc.CPUAccessFlags = 0;
	if( m_memHint == MEMORY_HINT_DYNAMIC ) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if( m_memHint == MEMORY_HINT_STAGING ){
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	desc.MiscFlags = 0;
	desc.StructureByteStride = (UINT)elementByteSize;
	device->CreateBuffer( &desc, nullptr, (ID3D11Buffer**) & m_handle);

	m_bufferByteSize = dataByteSize;
	m_elementByteSize = elementByteSize;

	if( m_handle != nullptr ) {
		//m_handle->SetPrivateData( WKPDID_D3DDebugObjectName, (size_t)m_debugName.size() , m_debugName.c_str() );
	}
	return ( m_handle != nullptr );
}
*/

/*
bool RenderBuffer::ValkunCreate( size_t dataByteSize, size_t elementByteSize )
{
	rendercontext
	return false;
}
*/

void RenderBuffer::Update( void const* data, size_t dataByteSize, size_t elementByteSize )
{
	// 1. if not compatible - destroy the old buffer
		// our elementSize matches the passed in 
	// 2. if no buffer, create one that is compatible
	// 3. updating the buffer
	if( !IsCompatible( dataByteSize, elementByteSize ) ) {
		Cleanup();// destroy the handle, reset things
		m_bufferByteSize = dataByteSize;
		m_elementByteSize = elementByteSize;
		m_owner->CreateRenderBuffer( *this );
	}
	m_owner->UpdateRenderBuffer( *this, data, dataByteSize, elementByteSize );
}


int RenderBuffer::GetMemoryUsage()
{
	switch( m_owner->GetRenderContextType() )
	{
		case RenderContextType::RENDER_CONTEXT_TYPE_D3D11:
			return GetDXMemoryUsage();
		case RenderContextType::RENDER_CONTEXT_TYPE_VULKAN:
			return GetVulkanMemoryUsage();
		default:
			break;
	}
}

int RenderBuffer::GetVulkanMemoryUsage()
{
	switch( m_memHint )
	{
		case MEMORY_HINT_GPU:		return VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		case MEMORY_HINT_DYNAMIC:	return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		case MEMORY_HINT_STAGING:	return VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		default:				ERROR_AND_DIE( "Unknown hint! " );
	}
}
