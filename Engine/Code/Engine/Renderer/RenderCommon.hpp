#pragma once
#include <string>
#include "Engine\Core\EngineCommon.hpp"

enum BufferFormatType
{
	// add types as you need them - for now, we can get by with just this
	BUFFER_FORMAT_VEC2,
	BUFFER_FORMAT_VEC3,
	BUFFER_FORMAT_R8G8B8A8_UNORM,
};

struct buffer_attribute_t
{
	std::string name; 			// used to link to a D3D11 shader
								// uint location; 			// used to link to a GL/Vulkan shader 
	BufferFormatType type;		// what data are we describing
	uint offset; 				// where is it relative to the start of a vertex

	buffer_attribute_t( char const* inputName, BufferFormatType inputType, uint inputOffset ) {
		name = inputName;
		type = inputType;
		offset = inputOffset;
	}
	buffer_attribute_t() { }
};