#include "IndexBuffer.hpp"

IndexBuffer::IndexBuffer( RenderContext* ctx, RenderMemoryHint hint )
	:RenderBuffer( ctx, INDEX_BUFFER_BIT, hint )
{
}

IndexBuffer::IndexBuffer( const char* debug, RenderContext* ctx, RenderMemoryHint hint )
	:RenderBuffer( ctx, INDEX_BUFFER_BIT, hint )
{

}

void IndexBuffer::Update( int iCount, const uint* indices )
{
	size_t dataByteSize = (size_t)( iCount * sizeof(uint) );
	RenderBuffer::Update( indices, dataByteSize, sizeof(uint) );
}

void IndexBuffer::Update( const std::vector<uint>& indices )
{

	Update( (int)indices.size(), indices.data() );
}
