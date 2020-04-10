#pragma once
#include <vector>
#include "Engine/Core/Transform.hpp"
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Renderer\RenderCommon.hpp"
#include "Engine\Renderer\RenderContext.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"

enum Vertex_Type {
	VERTEX_TYPE_PCU,
	VERTEX_TYPE_PCUTBN
};


class GPUMesh {
public:
	GPUMesh(){}
	~GPUMesh();
	GPUMesh( RenderContext* owner, Vertex_Type type=VERTEX_TYPE_PCU );

public:
	void UpdateVerticeBuffer();
	void UpdateVerticesInCPU( std::vector<Vertex_PCU> vertices );
	void UpdateTBNVerticesInCPU( std::vector<Vertex_PCUTBN> vertices );
	// void updateverticesInCPU( void const* vdata, size_t vstride, size_t vcount, buffer_attribute_t hello );
	// void UpdateGPUIfDirty();

	void UpdateIndiceBuffer();
	void UpdateIndicesInCPU( std::vector<uint> indices );

	// Accessor
	VertexBuffer* GetOrCreateVertexBuffer();
	IndexBuffer* GetOrCreateIndexBuffer(); 
	int GetVertexCount() const { return (int)m_vertices.size(); }
	int GetIndexCount() const { return (int)m_indices.size(); }

	// Mutator
	void SetType( Vertex_Type type );
	// helper template
// 	template <typename VERTEX_TYPE>
// 	void UpdateVerticeBuffer( int vCount, const VERTEX_TYPE* vertices ){
// 		UpdateVerticeBuffer( vCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT );
// 	}
	// using templete this friday
public:
	RenderContext* m_owner = nullptr;
	Vertex_Type m_type = VERTEX_TYPE_PCU;

private:
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indiceBuffer = nullptr;
	std::vector<Vertex_PCU> m_vertices;
	std::vector<Vertex_PCUTBN> m_TBNVertices;
	std::vector<uint> m_indices;

	// std::vector<unsigned char> m_cpuVertices;
	uint m_vertexStride;
	buffer_attribute_t const* m_layout;
	bool m_verticesDirty = false;
};