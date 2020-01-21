#include "SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"


SpriteDefinition::SpriteDefinition( const SpriteSheet& spriteSheet, int spriteIndex, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
	:m_spriteSheet(spriteSheet)
	,m_spriteIndex(spriteIndex)
	,m_uvAtMins(uvAtMins)
	,m_uvAtMaxs(uvAtMaxs)
{
	testing = uvAtMaxs.y;
}

void SpriteDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins.x = m_uvAtMins.x;
	out_uvAtMins.y = m_uvAtMins.y;
	out_uvAtMaxs.x = m_uvAtMaxs.x;
	out_uvAtMaxs.y = m_uvAtMaxs.y;
}

const SpriteSheet& SpriteDefinition::GetSpriteSheet() const
{
	return m_spriteSheet;
}

const Texture& SpriteDefinition::GetTexture() const
{
	return m_spriteSheet.GetTexture();
}

float SpriteDefinition::GetAspect() const
{
	float uvWidth = m_uvAtMaxs.x - m_uvAtMins.y;
	float uvHeight = m_uvAtMaxs.y - m_uvAtMins.y;
	Texture temTexture = m_spriteSheet.GetTexture();
	float temAspect = (temTexture.m_width * uvWidth) / (temTexture.m_height * uvHeight);
	return temAspect;
}

float SpriteDefinition::GetTesting() const
{
	return testing;
}
