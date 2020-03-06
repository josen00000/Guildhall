#include "BitmapFont.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/AABB2.hpp"

Vec2 BitmapFont::ALIGN_CENTERED = Vec2(0.5, 0.5);

BitmapFont::BitmapFont( const char* fontName, const Texture* fontTexture )
	:m_fontName(fontName)
	,m_glyphSpriteSheet (SpriteSheet(*fontTexture, IntVec2(16,16)))
{

}

const Texture* BitmapFont::GetTexture() const
{
	 const Texture& tem = m_glyphSpriteSheet.GetTexture();
	return &tem;
}



void BitmapFont::AddVertsForText2D( std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, const std::string& text, const Rgba8& tint, float cellAspect )
{	
	float glyphAspect = GetGlyphAspect(1);
	float cellWidth = cellHeight * glyphAspect * cellAspect;
	Vec2 minPos = textMins;
	float cellRightPosX = cellWidth;
	for(unsigned char c : text )
	{
		Vec2 minUV = Vec2(0,0);
		Vec2 maxUV = Vec2(0,0);
		m_glyphSpriteSheet.GetSpriteUVs(minUV,maxUV,c);

		Vertex_PCU leftDown = Vertex_PCU(minPos, tint, minUV );
		Vertex_PCU leftUp = Vertex_PCU(minPos + Vec2(0, cellHeight), tint, Vec2(minUV.x, maxUV.y) );
		Vertex_PCU rightUp = Vertex_PCU(minPos + Vec2(cellRightPosX, cellHeight), tint, maxUV );
		Vertex_PCU rightDown = Vertex_PCU(minPos + Vec2(cellRightPosX, 0), tint, Vec2(maxUV.x, minUV.y) );
		minPos.x += cellWidth;

		//push triangle A
		vertexArray.push_back(leftDown);
		vertexArray.push_back(leftUp);
		vertexArray.push_back(rightUp);
		//push triangle B
		vertexArray.push_back(leftDown);
		vertexArray.push_back(rightDown);
		vertexArray.push_back(rightUp);

	}
}

void BitmapFont::AddVertsForTextInBox2D( std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight, const std::string& text, const Rgba8& tint /*= Rgba8::WHITE*/, float cellAspect /*= 1.f*/, const Vec2& alignment /*= ALIGN_CENTERED*/ )
{
	float glyphAspect = GetGlyphAspect( 1 );
	float cellWidth = cellHeight * glyphAspect * cellAspect;
	float textLength = cellWidth * text.length();
	float maxX = box.maxs.x - textLength;
	float maxY = box.maxs.y - cellHeight;
	float minPosX = RangeMapFloat(0, 1, box.mins.x, maxX, alignment.x);
	float minPosY = RangeMapFloat(0, 1, box.mins.y, maxY, alignment.y);

	Vec2 minPos = Vec2(minPosX, minPosY);
	float cellRightPosX = cellWidth;
	for( unsigned char c : text )
	{
		Vec2 minUV = Vec2( 0, 0 );
		Vec2 maxUV = Vec2( 0, 0 );
		m_glyphSpriteSheet.GetSpriteUVs( minUV, maxUV, c );

		Vertex_PCU leftDown = Vertex_PCU( minPos, tint, minUV );
		Vertex_PCU leftUp = Vertex_PCU( minPos + Vec2( 0, cellHeight ), tint, Vec2( minUV.x, maxUV.y ) );
		Vertex_PCU rightUp = Vertex_PCU( minPos + Vec2( cellRightPosX, cellHeight ), tint, maxUV );
		Vertex_PCU rightDown = Vertex_PCU( minPos + Vec2( cellRightPosX, 0 ), tint, Vec2( maxUV.x, minUV.y ) );
		minPos.x += cellWidth;

		//push triangle A
		vertexArray.push_back( leftDown );
		vertexArray.push_back( leftUp );
		vertexArray.push_back( rightUp );
		//push triangle B
		vertexArray.push_back( leftDown );
		vertexArray.push_back( rightDown );
		vertexArray.push_back( rightUp );

	}
}

Vec2 BitmapFont::GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect /*= 1.f*/ )
{
	float glyphAspect = GetGlyphAspect(1);
	float cellWidth = cellHeight * glyphAspect * cellAspect;
	float textWidth = cellWidth * text.size();
	Vec2 result = Vec2( cellHeight, textWidth );
	return result;
}

float BitmapFont::GetGlyphAspect( int glyphUnicode ) const
{
	UNUSED (glyphUnicode);
	return 1;
}

