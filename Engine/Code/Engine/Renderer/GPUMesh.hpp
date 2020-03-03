#pragma once
#include <vector>
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Renderer\RenderCommon.hpp"
#include "Engine\Renderer\RenderContext.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"


class GPUMesh {
public:
	GPUMesh(){}
	~GPUMesh(){}

public:
	void UpdateVerticeBuffer( const buffer_attribute_t* layout );
	void UpdateVerticesInCPU( std::vector<Vertex_PCU> vertices );
	// void updateverticesInCPU( void const* vdata, size_t vstride, size_t vcount, buffer_attribute_t hello );
	// void UpdateGPUIfDirty();

	void UpdateIndiceBuffer();
	void UpdateIndicesInCPU( std::vector<uint> indices );

	// Accessor
	VertexBuffer* GetOrCreateVertexBuffer();
	IndexBuffer* GetOrCreateIndexBuffer(); 
	int GetVertexCount() const { return (int)m_vertices.size(); }
	int GetIndexCount() const { return (int)m_indices.size(); }

	// helper template
// 	template <typename VERTEX_TYPE>
// 	void UpdateVerticeBuffer( int vCount, const VERTEX_TYPE* vertices ){
// 		UpdateVerticeBuffer( vCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT );
// 	}

public:
	RenderContext* m_owner = nullptr;

private:
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indiceBuffer = nullptr;
	std::vector<Vertex_PCU> m_vertices;
	std::vector<uint> m_indices;
	
	// std::vector<unsigned char> m_cpuVertices;
	uint m_vertexStride;
	buffer_attribute_t const* layout;
	bool m_verticesDirty = false;

};