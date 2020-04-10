#pragma once
#include "Engine/Core/Vertex_PCU.hpp"

struct Vertex_PCUTBN {
public:
	Vertex_PCU pcu;
	Vec3 m_tangent;
	Vec3 m_bitangent;
	Vec3 m_normal;

	static buffer_attribute_t s_layout[];
public:
	Vertex_PCUTBN(){}
	~Vertex_PCUTBN(){}
	Vertex_PCUTBN( const Vertex_PCUTBN& copyFrom );
	explicit Vertex_PCUTBN( const Vertex_PCU& input_pcu, const Vec3& input_tangent,const Vec3& input_bitangent, const Vec3& input_normal );

	// operator
	bool operator==( const Vertex_PCUTBN& compareWith ) const;
};