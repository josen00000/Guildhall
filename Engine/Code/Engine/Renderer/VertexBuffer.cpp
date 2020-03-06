#include "VertexBuffer.hpp"
#include "RenderCommon.hpp"

VertexBuffer::VertexBuffer( RenderContext* ctx, RenderMemoryHint hint )
	:RenderBuffer( ctx, VERTEX_BUFFER_BIT, hint )
{

}


void VertexBuffer::UpdateVertices( int vCount, const void* vertices, buffer_attribute_t* attributes )
{
	size_t elementSize = 0;
	int i = 0;
	while( attributes[i].name == "" ){
		elementSize += attributes[i].offset;
		i++;
	}

	size_t dataByteSize = vCount * elementSize;
	RenderBuffer::Update( vertices, dataByteSize, elementSize );
}

