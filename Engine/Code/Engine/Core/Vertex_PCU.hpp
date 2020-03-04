#pragma once
#include<vector>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"

struct buffer_attribute_t;

struct Vertex_PCU {
public:
	Vec3 m_pos = Vec3();
	Vec2 m_uvTexCoords = Vec2();
	Rgba8 m_color = Rgba8();

	static buffer_attribute_t s_layout[]; 
public:
	Vertex_PCU(){}
	~Vertex_PCU(){}
	Vertex_PCU(const Vertex_PCU& copyFrom);
	explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords );
	void UpdatePos(Vec3 pos);

	// transformVertexArray();
	//gathering vertex and draw lines once
	//void AppendLine2D(std::vector<Vertex_PCU>& vertexArray,const Vec2&start, const Vec2& end, float thickness, const Rgba8& color);
//	void AppendAABB2D(std::vector<Vertex_PCU>& vertexArray, const AABB2& box, const Rgba8& color);

	// operator
	bool operator==( const Vertex_PCU& compareWith ) const;
};