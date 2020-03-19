#include "CubeSphere.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MeshUtils.hpp"


CubeSphere::CubeSphere( RenderContext* ctx, int level, AABB3 cube )
	:m_level(level)
	,m_cube(cube)
{
	m_mesh = new GPUMesh();
	m_mesh->m_owner = ctx;
	std::vector<Vertex_PCU> vertices;
	std::vector<uint> indices;
	AppendIndexedVertsForCubeSphere3D( vertices, indices, m_cube, m_level );
	m_mesh->UpdateVerticesInCPU( vertices );
	m_mesh->UpdateIndicesInCPU( indices );
}

CubeSphere::~CubeSphere()
{
	SELF_SAFE_RELEASE(m_mesh);
}
