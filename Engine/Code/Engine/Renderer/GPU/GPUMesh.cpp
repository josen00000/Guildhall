#include "GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/GPU/IndexBuffer.hpp"
#include "Engine/Renderer/GPU/VertexBuffer.hpp"
#include "Engine/Renderer/D3D11Common.hpp"


GPUMesh::~GPUMesh()
{
	SELF_SAFE_RELEASE(m_vertexBuffer);
	SELF_SAFE_RELEASE(m_indiceBuffer);
}


GPUMesh::GPUMesh( RenderContext* owner, Vertex_Type type/*=VERTEX_TYPE_PCU */ )
	:m_owner(owner)
	,m_type(type)
{
}

void GPUMesh::UpdateVerticeBuffer( )
{
	GetOrCreateVertexBuffer();
	size_t dataByteSize;
	size_t elementByteSize;
	switch( m_type )
	{
	case VERTEX_TYPE_PCU:
		elementByteSize = sizeof(Vertex_PCU);
		dataByteSize = (size_t)( m_vertices.size() * elementByteSize );
		m_vertexBuffer->Update( &m_vertices[0], dataByteSize, elementByteSize );
		m_vertexBuffer->SetLayout( Vertex_PCU::s_layout );
		break;
	case VERTEX_TYPE_PCUTBN:
		elementByteSize = sizeof(Vertex_PCUTBN);
		dataByteSize = (size_t)( m_TBNVertices.size() * elementByteSize );
		m_vertexBuffer->Update( m_TBNVertices.data(), dataByteSize, elementByteSize );
		
		m_vertexBuffer->SetLayout( Vertex_PCUTBN::s_layout );
		break;
	default:
		break;
	}
}

void GPUMesh::UpdateVerticesInCPU( std::vector<Vertex_PCU> vertices )
{
	m_vertices = vertices;
}

void GPUMesh::UpdateTBNVerticesInCPU( std::vector<Vertex_PCUTBN> vertices )
{
	m_TBNVertices = vertices;
}

void GPUMesh::UpdateIndiceBuffer()
{
	GetOrCreateIndexBuffer( m_debugMsg.c_str() );
	m_indiceBuffer->Update( m_indices );
}

void GPUMesh::UpdateIndicesInCPU( std::vector<uint> indices )
{
	m_indices = indices;
}

VertexBuffer* GPUMesh::GetOrCreateVertexBuffer()
{
	if( m_vertexBuffer != nullptr ){
		return m_vertexBuffer;
	}

	GUARANTEE_OR_DIE ( m_owner != nullptr, "GPU Mesh owner is nullptr!" );
	m_vertexBuffer = new VertexBuffer( m_owner, MEMORY_HINT_DYNAMIC  );
	return m_vertexBuffer;
}

IndexBuffer* GPUMesh::GetOrCreateIndexBuffer()
{
	if( m_indiceBuffer != nullptr ){
		return m_indiceBuffer;
	}

	GUARANTEE_OR_DIE( m_owner != nullptr, "GPU Mesh owner is nullptr!" );
	m_indiceBuffer = new IndexBuffer( m_owner, MEMORY_HINT_DYNAMIC );
	return m_indiceBuffer;
}

IndexBuffer* GPUMesh::GetOrCreateIndexBuffer( const char* debug )
{
	if( m_indiceBuffer != nullptr ) {
		return m_indiceBuffer;
	}

	GUARANTEE_OR_DIE( m_owner != nullptr, "GPU Mesh owner is nullptr!" );
	m_indiceBuffer = new IndexBuffer( debug, m_owner, MEMORY_HINT_DYNAMIC );
	return m_indiceBuffer;
}

void GPUMesh::SetType( Vertex_Type type )
{
	m_type = type;
}

