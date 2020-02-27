#pragma once
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderCommon.hpp"

class RenderContext;
struct ID3D10Bolb;
struct ID3D11InputLayout;
struct ID3D11PixelShader;
struct ID3D11RasterizerState;
struct ID3D11Resource;
struct ID3D11VertexShader;

enum ShaderType {
	SHADER_TYPE_VERTEX,
	SHADER_TYPE_FRAGMENT,
};

class ShaderStage {

public:
	~ShaderStage();
	bool Compile( RenderContext* ctx,
		std::string const& fileName, // for debug
		void const* source, //shader code
		size_t const sourceByteLen,
		ShaderType stage
		); //
	bool IsValid() const { return ( nullptr != m_handle); }

	void const* GetByteCode() const;
	size_t GetByteCodeLength() const;


public:
	ID3D10Blob* m_byteCode;
	union {
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_fs;
	};
	ShaderType m_type;
};

class Shader {
public:
	Shader( RenderContext* owner );
	~Shader();
	bool CreateFromFile( std::string const& fileName );
	void CreateRasterState();
	ID3D11InputLayout* GetOrCreateInputLayout( /*buffer*/);
	DXGI_FORMAT TransformToD3DDataFormat( BufferFormatType type );
public:
	RenderContext*	m_owner	= nullptr;
	ShaderStage m_vertexStage;
	ShaderStage m_fragmentStage;
	ID3D11RasterizerState* m_rasterState = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
};