#include <d3dcompiler.h>
#include <stdio.h>

#include "Shader.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/D3D11Common.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Shader.hpp"


void* FileReadToNewBuffer( std::string const& fileName, size_t* out_size ) {
	
	FILE* fp = nullptr;
	fopen_s( &fp, fileName.c_str(), "r" );
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


bool ShaderStage::Compile( RenderContext* ctx, std::string const& fileName, /* for debug */ void const* source, /*shader code */ size_t const sourceByteLen, ShaderType stage )
{
	UNUSED(ctx);
	UNUSED(fileName);
	UNUSED(sourceByteLen);
	UNUSED(source);
	UNUSED(stage);
	return false;
}

bool Shader::CreateFromFile( std::string const& fileName )
{
	size_t file_size = 0;
	void* src = FileReadToNewBuffer( fileName, &file_size );
	if( src == nullptr ) {
		return false;
	}

	delete[] src;
	return false;
}
