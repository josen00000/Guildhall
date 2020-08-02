#include "SpriteSheet.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Renderer/Texture.hpp"


SpriteSheet::SpriteSheet( Texture* texture, const IntVec2& simpleGridLayout )
	:m_texture(texture)
	,m_layout(simpleGridLayout)
{
	CreateSpriteDefinitions( simpleGridLayout );
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


void SpriteSheet::GetSpriteUVsWithCoords( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords ) const
{
	int index = GetIndexWithCoords( spriteCoords );
	GetSpriteUVs( out_uvAtMins, out_uvAtMaxs, index );
}

int SpriteSheet::GetIndexWithCoords( IntVec2 spriteCoords ) const
{
	int index = spriteCoords.x + spriteCoords.y * m_layout.x;
	return index;
}

int SpriteSheet::GetIndexWithCoords( int spriteX, int spriteY ) const
{
	return GetIndexWithCoords( IntVec2( spriteX, spriteY ));
}

void SpriteSheet::CreateSpriteDefinitions( const IntVec2 simpleGridLayout )
{
	float temWidth = 1 / (float)simpleGridLayout.x;
	float temHeight = 1 / (float)simpleGridLayout.y;

	for( int spriteY = 0; spriteY < simpleGridLayout.y; spriteY++ ) {
		for( int spriteX = 0; spriteX < simpleGridLayout.x; spriteX++ ) {
			int spriteIndex = spriteX + spriteY * simpleGridLayout.x;
			Vec2 uvAtMins = Vec2( temWidth * spriteX , 1 - ( temHeight * ( spriteY + 1 )));
			Vec2 uvAtMaxs = Vec2( temWidth * (spriteX + 1) , 1 - ( temHeight * spriteY ));
			IntVec2 textureSize = m_texture->GetSize();
			const SpriteDefinition tem = SpriteDefinition( textureSize, uvAtMins, uvAtMaxs );
			m_spriteDefs.push_back(tem);
		}
	}
}

