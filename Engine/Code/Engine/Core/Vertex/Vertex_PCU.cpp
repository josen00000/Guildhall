#include "Engine/Core/Vertex/Vertex_PCU.hpp"
#include "Engine/Renderer/RenderCommon.hpp"


buffer_attribute_t Vertex_PCU::s_layout [4] = {
	buffer_attribute_t( "POSITION",	BUFFER_FORMAT_VEC3,				offsetof( Vertex_PCU, m_pos ) ),
	buffer_attribute_t( "COLOR",	BUFFER_FORMAT_R8G8B8A8_UNORM,	offsetof( Vertex_PCU, m_color ) ),
	buffer_attribute_t( "TEXCOORD", BUFFER_FORMAT_VEC2,				offsetof( Vertex_PCU, m_uvTexCoords ) ),
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

Vertex_PCU Vertex_PCU::GetMiddleVertForTwoVerts( const Vertex_PCU& A, const Vertex_PCU& B )
{
	Vertex_PCU result;
	result.m_pos = ( A.m_pos + B.m_pos ) / 2;
	result.m_uvTexCoords = ( A.m_uvTexCoords + B.m_uvTexCoords ) / 2;
	result.m_color = Rgba8( ( A.m_color.r + B.m_color.r ) / 2, ( A.m_color.g + B.m_color.g ) / 2, ( A.m_color.b + B.m_color.b ) / 2, ( A.m_color.a + B.m_color.a ) / 2 );
	return result;
}

bool Vertex_PCU::operator==( const Vertex_PCU& compareWith ) const
{
	if( m_pos == compareWith.m_pos && m_uvTexCoords == compareWith.m_uvTexCoords && m_color == compareWith.m_color ){
		return true;
	}
	else{
		return false;
	}
}

