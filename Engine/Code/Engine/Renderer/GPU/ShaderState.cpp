#include "ShaderState.hpp"


ShaderState::ShaderState( RenderContext* ctx, std::string statePath )
	:m_owner(ctx)
	,m_statePath(statePath)
{
	LoadShaderState();
}

void ShaderState::SetBlendMode( BlendMode mode )
{
	m_blendMode = mode;
}

void ShaderState::SetDepth( DepthCompareFunc compFunc, bool ableWriteDepth )
{
	m_depthFunc = compFunc;
	m_ableWriteDepth = ableWriteDepth;
}

void ShaderState::SetCullMode( RasterCullMode mode )
{
	m_cullMode = mode;
}

void ShaderState::SetFillMode( RasterFillMode mode )
{
	m_fillMode = mode;
}

void ShaderState::PrepareForDraw()
{
	m_owner->SetCullMode( m_cullMode );
	m_owner->EnableDepth( m_depthFunc, m_ableWriteDepth );
	m_owner->SetFillMode( m_fillMode );
	m_owner->BindShader( m_shaderPath );
}

void ShaderState::LoadShaderState()
{
	XmlDocument shaderStateDoc;
	shaderStateDoc.LoadFile( m_statePath.c_str() );

	if( IfLoadXmlFileSucceed( shaderStateDoc ) ) {
		XmlElement* rootElement = shaderStateDoc.RootElement();
		XmlElement* childElement = rootElement->FirstChildElement();
		
		m_name			= ParseXmlAttribute( *childElement, "name", "UNNAMED" );
		m_shaderPath	= ParseXmlAttribute( *childElement, "path", "" );

		childElement = childElement->FirstChildElement();
		while( childElement ) {
			ParseMode( childElement );
			childElement = childElement->NextSiblingElement();
		}
	}
}

void ShaderState::ParseMode( XmlElement* element )
{
	std::string name = element->Name();
	if( name.compare( "Blend" ) == 0 ) {
		std::string tempMode = ParseXmlAttribute( *element, "mode", "alpha" );
		if( tempMode.compare( "alpha" ) == 0 ) {
			m_blendMode = BLEND_ALPHA;
		}
		else if( tempMode.compare( "additive" ) == 0 ) {
			m_blendMode = BLEND_ADDITIVE;
		}
		else if( tempMode.compare( "opaque" ) == 0 ) {
			m_blendMode = BLEND_OPAQUE;
		}
	}
	else if( name.compare( "Cull" ) == 0 ) {
		std::string tempMode = ParseXmlAttribute( *element, "mode", "back" );
		if( tempMode.compare( "back" ) == 0 ) {
			m_cullMode = RASTER_CULL_BACK;
		}
		else if( tempMode.compare( "front" ) == 0 ) {
			m_cullMode = RASTER_CULL_FRONT;
		}
		else if( tempMode.compare( "none" ) == 0 ) {
			m_cullMode = RASTER_CULL_NONE;
		}
	}
	else if( name.compare( "Fill" ) == 0 ) {
		std::string tempMode = ParseXmlAttribute( *element, "mode", "solid" );
		if( tempMode.compare( "solid" ) == 0 ) {
			m_fillMode = RASTER_FILL_SOLID;
		}
		else if( tempMode.compare( "wireframe" ) == 0 ) {
			m_fillMode = RASTER_FILL_WIREFRAME;
		}
	}
	else if( name.compare( "Depth" ) == 0 ) {
		std::string ableDepth = ParseXmlAttribute( *element, "isAble", "true" );
		m_ableDepth = GetBoolFromText( ableDepth.c_str() );

		std::string ableWriteDepth = ParseXmlAttribute( *element, "writeDepth", "true" );
		m_ableWriteDepth = GetBoolFromText( ableWriteDepth.c_str() );

		std::string compFunc = ParseXmlAttribute( *element, "compFunc", "less_equal" );
		if( compFunc.compare( "less_equal" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_LESS_EQUAL;
		}
		else if( compFunc.compare( "less" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_LESS;
		}
		else if( compFunc.compare( "always" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_ALWAYS;
		}
		else if( compFunc.compare( "never" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_NEVER;
		}
		else if( compFunc.compare( "equal" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_EQUAL;
		}
		else if( compFunc.compare( "greater" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_GREATER;
		}
		else if( compFunc.compare( "not_equal" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_NOT_EQUAL;
		}
		else if( compFunc.compare( "greater_equal" ) == 0 ) {
			m_depthFunc = COMPARE_DEPTH_GREATER_EQUAL;
		}
	}
}
