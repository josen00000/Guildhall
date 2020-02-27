#pragma once
#include "Engine\Renderer\IndexBuffer.hpp"
#include "Engine\Renderer\RenderCommon.hpp"
#include "Engine\Renderer\VertexBuffer.hpp"

class GPUMesh {
public:
	void UpdateVertices( int vCount, const void* vertexData, int vertexStride, const buffer_attribute_t* layout );
	void UpdateIndices( int iCount, const int* indices );

	int GetIndexCount() const;

	// helper template
	template <typename VERTEX_TYPE>
	void UpdateVertices( int vCount, const VERTEX_TYPE* vertices ){
		UpdateVertices( vCount, vertices, sizeof(VERTEX_TYPE), VERTEX_TYPE::LAYOUT );
	}

public:
	VertexBuffer* m_vertices = nullptr;
	IndexBuffer* m_indices = nullptr;
};