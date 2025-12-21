#pragma once
#include <vector>
#include "Engine/Core/Rgba8.hpp"

class Sampler;
class ShaderState;
class Texture;
class RenderBuffer;

class Material {
friend class RenderContext;

public:
	Material(){}
	~Material();
public:
	// accessor
	template<typename DataType>
	void GetDataAs();
	bool IsUBODirty() const { return m_uboIsDirty; }

	// mutator
	template<typename DataType>
	void SetData( const DataType& data );
	void AddTexture( Texture* tex );
	void SetShaderState( ShaderState* state );

private:
	void UpdateUniformBuffer();
	void SetData( const void* data, size_t dataSize );

private:
	bool m_uboIsDirty = false;
	Rgba8 m_tint;
	float m_specularFactor;
	float m_specularPower;

	ShaderState* m_shaderState	= nullptr;
	Sampler* m_sampler			= nullptr;

	std::vector<Texture*> m_texturePerSlot;
	std::vector<Sampler*> m_samplerPerSlot;
	std::vector<unsigned char> m_uboCPUData;

	RenderBuffer* m_ubo = nullptr;

};

template<typename DataType>
void Material::GetDataAs()
{
	m_uboIsDirty = true;
	if( m_uboCPUData.size() = sizeof( DataType ) ) {
		return (DataType*)&m_uboCPUData.data();
	}
	else {
		m_uboCPUData.resize( sizeof(DataType) );
		DataType* retPtr = (DataType*)&m_uboCPUData.data();
		new (retPtr) DataType();
		return retPtr;
	}
}

template<typename DataType>
void Material::SetData( const DataType& data )
{
	SetData( data, sizeof(DataType) );
}
