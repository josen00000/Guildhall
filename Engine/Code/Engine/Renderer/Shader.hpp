#pragma once
#include "Engine/Core/StringUtils.hpp"

class RenderContext;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Resource;

enum ShaderType {
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
};

class ShaderStage {
public:
	ShaderType m_type;

public:
	bool Compile( RenderContext* ctx,
		std::string const& fileName, // for debug
		void const* source, //shader code
		size_t const sourceByteLen,
		ShaderType stage
		); //


public:
	union {
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_fs;
	};

};

class Shader {
public:
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;

public:
	bool CreateFromFile( std::string const& fileName );


};