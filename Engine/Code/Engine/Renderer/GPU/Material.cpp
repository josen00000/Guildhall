#include "Material.hpp"
#include "Engine/Renderer/GPU/RenderBuffer.hpp"
#include "Engine/Renderer/GPU/Texture.hpp"
#include "Engine/Renderer/GPU/Sampler.hpp"
#include "Engine/Renderer/GPU/ShaderState.hpp"


void Material::UpdateUniformBuffer()
{
	if( m_ubo != nullptr ) {
		m_ubo->Update( m_uboCPUData.data(), m_uboCPUData.size(), m_uboCPUData.size() );
	}
}

Material::~Material()
{
	for( int i = 0; i < m_texturePerSlot.size(); i++ ) {
		SELF_SAFE_RELEASE( m_texturePerSlot[i] );
	}
	SELF_SAFE_RELEASE(m_ubo);
	SELF_SAFE_RELEASE(m_sampler);
	SELF_SAFE_RELEASE(m_shaderState);
}

void Material::AddTexture( Texture* tex )
{
	m_texturePerSlot.push_back( tex );
}

void Material::SetShaderState( ShaderState* state )
{
	m_shaderState = state;	
}

void Material::SetData( const void* data, size_t dataSize )
{
	m_uboCPUData.resize( dataSize );
	memcpy( &m_uboCPUData[0], data, dataSize );
	m_uboIsDirty = true;
}
