
#include<Engine/Core/Vertex_PCU.hpp>




Vertex_PCU::Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords )
	:m_pos(position)
	,m_color(tint)
	,m_uvTexCoords(uvTexCoords)
{

}

Vertex_PCU::Vertex_PCU( const Vertex_PCU& copyFrom )
	:m_pos( copyFrom.m_pos )
	, m_color(copyFrom.m_color )
	, m_uvTexCoords( copyFrom.m_uvTexCoords )
{

}


void Vertex_PCU::UpdatePos( Vec3 pos )
{
	m_pos.operator+=(pos);
}

