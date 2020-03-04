#include "GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"


void GPUMesh::UpdateVerticeBuffer( const buffer_attribute_t* layout  )
{
	UNUSED(layout);
	GetOrCreateVertexBuffer();
	size_t dataByteSize = (size_t)(m_vertices.size() * sizeof(Vertex_PCU) );
	m_vertexBuffer->Update( &m_vertices[0], dataByteSize, sizeof(Vertex_PCU) );
}

void GPUMesh::UpdateVerticesInCPU( std::vector<Vertex_PCU> vertices )
{
	m_vertices = vertices;
}

void GPUMesh::UpdateIndiceBuffer()
{
	GetOrCreateIndexBuffer();
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

