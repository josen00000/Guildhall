#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderCommon.hpp"


buffer_attribute_t Vertex_PCU::s_layout [4] = {
	buffer_attribute_t( "POSITION",	BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCU, m_pos ) ),
	buffer_attribute_t( "COLOR",	BUFFER_FORMAT_R8G8B8A8_UNORM,				offsetof( Vertex_PCU, m_color ) ),
	buffer_attribute_t( "TEXCOORD", BUFFER_FORMAT_VEC2,	offsetof( Vertex_PCU, m_uvTexCoords ) ),
	buffer_attribute_t() // end - terminator element; 
};


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

