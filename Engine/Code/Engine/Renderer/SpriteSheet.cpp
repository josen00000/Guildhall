#include "SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/vec2.hpp"



SpriteSheet::SpriteSheet( Texture& texture, const IntVec2& simpleGridLayout )
	:m_texture(texture)
	,m_layout(simpleGridLayout)
{
	CreateSpriteDefinitions(simpleGridLayout);
}

const SpriteDefinition& SpriteSheet::GetSpriteDefinition( int spriteIndex ) const
{
	const SpriteDefinition& tem = m_spriteDefs[spriteIndex];
	return tem;
}

void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex ) const
{
	const SpriteDefinition& tem = GetSpriteDefinition(spriteIndex);
	tem.GetUVs(out_uvAtMins, out_uvAtMaxs);
}


void SpriteSheet::GetSpriteUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords ) const
{
	int spriteIndex = spriteCoords.x + spriteCoords.y * m_layout.x;
	const SpriteDefinition& tem = GetSpriteDefinition( spriteIndex );
	tem.GetUVs( out_uvAtMins, out_uvAtMaxs );
}

void SpriteSheet::CreateSpriteDefinitions( const IntVec2 simpleGridLayout )
{
	float temWidth = 1 / (float)simpleGridLayout.x;
	float temHeight = 1 / (float)simpleGridLayout.y;
	for( int spriteY = 0; spriteY < simpleGridLayout.y; spriteY++ ) {
		for( int spriteX = 0; spriteX < simpleGridLayout.x; spriteX++ ) {
			int spriteIndex = spriteX + spriteY * simpleGridLayout.x;
			Vec2 uvAtMins = Vec2( temWidth * spriteX , 1- temHeight * (spriteY + 1));
			Vec2 uvAtMaxs = Vec2( temWidth * (spriteX + 1) , 1- temHeight * spriteY);
			const SpriteDefinition tem= SpriteDefinition(*this, spriteIndex, uvAtMins, uvAtMaxs );
			m_spriteDefs.push_back(tem);
		}
	}
}

