#include <d3dcompiler.h>
#include <stdio.h>

#include "Shader.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"


void* FileReadToNewBuffer( std::string const& fileName, size_t* out_size ) {
	FILE* fp = nullptr;
	fopen_s( &fp, fileName.c_str(), "r" );
	GUARANTEE_RECOVERABLE( fp != nullptr, "Create Shader with wrong name." );
	if( fp == nullptr ){ return nullptr; }

	fseek( fp, 0, SEEK_END );
	long file_size = ftell( fp );

	unsigned char* buffer = new unsigned char[file_size + 1];
	if( nullptr != buffer ) {
		fseek( fp, 0, SEEK_SET );
		size_t bytes_read =  fread( buffer, 1, file_size, fp );
		buffer[bytes_read] = NULL;
	}

	if( out_size != nullptr ) {
		*out_size = (size_t)file_size;
	}

	fclose( fp );
	return buffer;
}

static char const* GetDefaultEntryPointForStage( ShaderType type ) {
	switch( type )
	{
		case SHADER_TYPE_VERTEX:
			return "VertexFunction"; // The function name in shader
		case SHADER_TYPE_FRAGMENT:
			return "FragmentFunction";
		default: GUARANTEE_OR_DIE( false,  "Bad Stage" );
	}
}

static char const* GetShaderModelForStage( ShaderType type ) {
	switch (type)
	{
		case SHADER_TYPE_VERTEX: return "vs_5_0";
		case SHADER_TYPE_FRAGMENT: return "ps_5_0";
		default: GUARANTEE_OR_DIE( false, "Unknown shader stage" );
	}
}

ShaderStage::~ShaderStage()
{
	DX_SAFE_RELEASE(m_byteCode);
	DX_SAFE_RELEASE(m_handle);
}

bool ShaderStage::Compile( RenderContext* ctx, std::string const& fileName, /* for debug */ void const* source, /*shader code */ size_t const sourceByteLen, ShaderType stage )
{
	// HLSL - High level shading language
	// Compile : HLSL -> Bytecode
	// Link ByteCode to device assembly

	char const* entryPoint = GetDefaultEntryPointForStage( stage );
	char const* shaderModel = GetShaderModelForStage( stage );

	DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_DEBUG;
#else
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	// Binary large Physics
	ID3DBlob* byteCode	= nullptr;
	ID3DBlob* errors		= nullptr;

	HRESULT hr = ::D3DCompile( source,
		sourceByteLen,
		fileName.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entryPoint,			//first function get called in program
		shaderModel,		//target( MSDN - specifying compiler targets)
		compile_flags,
		0,
		&byteCode,
		&errors );

	if( FAILED( hr ) ) {
		if( errors != nullptr ) {
			char* errorString = (char*) errors->GetBufferPointer();
			DebuggerPrintf( "Failed to compile [%s]. Compiler gave the following output; \n%s",
				fileName.c_str(),
				errorString );
		}
		DebugBreak();
	}
	else {
		ID3D11Device* device = ctx->m_device;
		void const* byteCodePtr = byteCode->GetBufferPointer();
		size_t byteCodeSize = byteCode->GetBufferSize();
		switch( stage )
		{
			default: GUARANTEE_OR_DIE( false, "Unimplemented stage." );
			case SHADER_TYPE_VERTEX: {
				hr = device->CreateVertexShader( byteCodePtr, byteCodeSize, nullptr, &m_vs );
				GUARANTEE_OR_DIE( SUCCEEDED(hr), "Failed to link shader stage" );
				break;
			}
			case SHADER_TYPE_FRAGMENT: {
				hr = device->CreatePixelShader( byteCodePtr, byteCodeSize, nullptr, &m_fs );
				GUARANTEE_OR_DIE( SUCCEEDED(hr), "Failed to link shader stage" );
				break;
			}
		}
	}

	DX_SAFE_RELEASE(errors); // always errors

	if( stage == SHADER_TYPE_VERTEX ) {
		m_byteCode = byteCode;
	}
	else {
		DX_SAFE_RELEASE(byteCode);
		m_byteCode = nullptr;
	}

	m_type = stage;	
	return IsValid();
}

void const* ShaderStage::GetByteCode() const
{
	return m_byteCode->GetBufferPointer();
}

size_t ShaderStage::GetByteCodeLength() const
{
	return m_byteCode->GetBufferSize();
}

Shader::Shader( RenderContext* owner )
	:m_owner(owner)
{
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_d3dInputLayout);
	m_owner = nullptr;
}

bool Shader::CreateFromFile( std::string const& fileName )
{
	size_t file_size = 0;
	void* source = FileReadToNewBuffer( fileName, &file_size );
	if( source == nullptr ) {
		return false;
	}

	m_vertexStage.Compile( m_owner, fileName, source, file_size, SHADER_TYPE_VERTEX );
	m_fragmentStage.Compile( m_owner, fileName, source, file_size, SHADER_TYPE_FRAGMENT );

	delete[] source;
	return m_vertexStage.IsValid() && m_fragmentStage.IsValid();
}

ID3D11InputLayout* Shader::GetOrCreateInputLayout( VertexBuffer* vbo )
{
	if( vbo->m_attributes == m_attribute && m_d3dInputLayout != nullptr ){ return m_d3dInputLayout; }

	const buffer_attribute_t* vboLayout = vbo->m_attributes;
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDescs;

	int i = 0;
	while( vboLayout[i].name != "" ){
		D3D11_INPUT_ELEMENT_DESC vertexDesc;
		vertexDesc.SemanticName			= vboLayout[i].name.c_str();
		vertexDesc.SemanticIndex		= 0;							//	Array for semantic name; 
		vertexDesc.Format				= TransformToD3DDataFormat( vboLayout[i].type );	// what in 
		vertexDesc.InputSlot			= 0;							// 
		vertexDesc.AlignedByteOffset	= vboLayout[i].offset;	// 
		vertexDesc.InputSlotClass		= D3D11_INPUT_PER_VERTEX_DATA;	// vertex and instance 
		vertexDesc.InstanceDataStepRate	= 0;
		vertexDescs.push_back( vertexDesc );
		i++;
	}

	ID3D11Device* device = m_owner->m_device;
	DX_SAFE_RELEASE(m_d3dInputLayout); // what is error. need to figure out
	device->CreateInputLayout(
		vertexDescs.data(), (uint)vertexDescs.size(), //describe the vertex
		m_vertexStage.GetByteCode(), m_vertexStage.GetByteCodeLength(),
		&m_d3dInputLayout );

	return m_d3dInputLayout;
}

DXGI_FORMAT Shader::TransformToD3DDataFormat( BufferFormatType type )
{
	switch( type )
	{
	case BUFFER_FORMAT_VEC2: return DXGI_FORMAT_R32G32_FLOAT;
	case BUFFER_FORMAT_VEC3: return DXGI_FORMAT_R32G32B32_FLOAT;
	case BUFFER_FORMAT_R8G8B8A8_UNORM: return DXGI_FORMAT_R8G8B8A8_UNORM;
	default:
		return DXGI_FORMAT_R32G32_FLOAT;
	}
}
