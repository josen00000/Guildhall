#include "SpriteDefinition.hpp"
#include "Engine/Renderer/Texture.hpp"


SpriteDefinition::SpriteDefinition( IntVec2 sheetSize, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
	:m_sheetSize(sheetSize)
	, m_uvAtMins(uvAtMins)
	, m_uvAtMaxs(uvAtMaxs)
{
}

void SpriteDefinition::GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const
{
	out_uvAtMins.x = m_uvAtMins.x;
	out_uvAtMins.y = m_uvAtMins.y;
	out_uvAtMaxs.x = m_uvAtMaxs.x;
	out_uvAtMaxs.y = m_uvAtMaxs.y;
}

float SpriteDefinition::GetAspect() const
{
	float uvWidth = m_uvAtMaxs.x - m_uvAtMins.y;
	float uvHeight = m_uvAtMaxs.y - m_uvAtMins.y;
	float temAspect = ( m_sheetSize.x * uvWidth ) / ( m_sheetSize.y * uvHeight );
	return temAspect;
}
