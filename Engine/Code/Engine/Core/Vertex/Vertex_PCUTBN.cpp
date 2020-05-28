#include "Vertex_PCUTBN.hpp"
#include "Engine/Renderer/RenderCommon.hpp"


buffer_attribute_t Vertex_PCUTBN::s_layout[7] = {
	buffer_attribute_t( "POSITION",	BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, m_pcu.m_pos ) ),
	buffer_attribute_t( "COLOR",	BUFFER_FORMAT_R8G8B8A8_UNORM,	offsetof( Vertex_PCUTBN, m_pcu.m_color ) ),
	buffer_attribute_t( "TEXCOORD", BUFFER_FORMAT_VEC2,				offsetof( Vertex_PCUTBN, m_pcu.m_uvTexCoords ) ),
	buffer_attribute_t( "TANGENT", BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, m_tangent ) ),
	buffer_attribute_t( "BITANGENT", BUFFER_FORMAT_VEC3,			offsetof( Vertex_PCUTBN, m_bitangent ) ),
	buffer_attribute_t( "NORMAL",	BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCUTBN, m_normal ) ),
	buffer_attribute_t()
};

Vertex_PCUTBN::Vertex_PCUTBN( const Vertex_PCUTBN& copyFrom )
	:m_pcu(copyFrom.m_pcu)
	,m_normal(copyFrom.m_normal)
	,m_tangent(copyFrom.m_tangent)
	,m_bitangent(copyFrom.m_bitangent)
{
}

Vertex_PCUTBN::Vertex_PCUTBN( const Vertex_PCU& input_pcu, const Vec3& input_tangent, const Vec3& input_bitangent, const Vec3& input_normal )
	:m_pcu( input_pcu )
	,m_normal(input_normal)
	,m_tangent(input_tangent)
	,m_bitangent(input_bitangent)
{
}

// operator
bool Vertex_PCUTBN::operator==( const Vertex_PCUTBN& compareWith ) const
{
	return ( m_pcu == compareWith.m_pcu && m_normal == compareWith.m_normal );
}
