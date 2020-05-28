#pragma once
#include "Engine/Renderer/GPU/RenderBuffer.hpp"
#include <vector>


class IndexBuffer : public RenderBuffer {
public:
	IndexBuffer( RenderContext* ctx, RenderMemoryHint hint );
	IndexBuffer( const char* debug, RenderContext* ctx, RenderMemoryHint hint );

public:
	void Update( const std::vector<uint>& indices );

private:
	void Update( int iCount, const uint* indices );
};