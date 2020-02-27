#include "VertexBuffer.hpp"

VertexBuffer::VertexBuffer( RenderContext* ctx, RenderMemoryHint hint )
	:RenderBuffer( ctx, VERTEX_BUFFER_BIT, hint )
{

}

