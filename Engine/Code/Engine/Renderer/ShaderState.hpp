#pragma once
#include <string>
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/ThirdParty/XmlUtils.hpp"

class ShaderState {
public:
	ShaderState()=delete;
	~ShaderState(){}
	explicit ShaderState( RenderContext* ctx, std::string statePath );

	// mutator
	void SetBlendMode( BlendMode mode );
	void SetDepth( DepthCompareFunc compFunc, bool ableWriteDepth );
	void SetCullMode( RasterCullMode mode );
	void SetFillMode( RasterFillMode mode );
	void PrepareForDraw();

private:
	void LoadShaderState();
	void ParseMode( XmlElement* element );
private:
	RenderContext* m_owner = nullptr;
	std::string m_shaderPath;
	std::string m_name;
	std::string m_statePath;
	bool m_ableDepth	= true;
	bool m_ableWriteDepth = true;
	BlendMode m_blendMode = BLEND_OPAQUE;
	DepthCompareFunc m_depthFunc = COMPARE_DEPTH_LESS_EQUAL;
	RasterCullMode m_cullMode = RASTER_CULL_NONE;
	RasterFillMode m_fillMode = RASTER_FILL_SOLID;
};