#pragma once
#include<vector>
#include <array>
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/vec2.hpp"

struct buffer_attribute_t;
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

struct Vertex_PCU {
public:
	Vec3 m_pos = Vec3();
	Vec2 m_uvTexCoords = Vec2();
	Rgba8 m_color = Rgba8();

	// d3d11
	static buffer_attribute_t s_layout[]; 

	// vulkan
	static VkVertexInputBindingDescription GetBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 3> GetAttributeDescriptions();

public:
	Vertex_PCU(){}
	~Vertex_PCU(){}
	Vertex_PCU( const Vertex_PCU& copyFrom );
	explicit Vertex_PCU( const Vec3& position, const Rgba8& tint, const Vec2& uvTexCoords );
	void UpdatePos(Vec3 pos);

	static Vertex_PCU GetMiddleVertForTwoVerts( const Vertex_PCU& A, const Vertex_PCU& B );

	// operator
	bool operator==( const Vertex_PCU& compareWith ) const;
};