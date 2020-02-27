#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"
#include <vector>


class IndexBuffer : public RenderBuffer {
public:
	void Update( int iCount, const int* indices );
	void Update( const std::vector<int>& indices );
};