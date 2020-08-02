#pragma once
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/IntVec2.hpp"

class Texture;

class SpriteDefinition
{
public:
	explicit SpriteDefinition( IntVec2 sheetSize, const Vec2& uvAtMins, const Vec2& uvAtMaxs );
	void				GetUVs( Vec2& out_uvAtMins, Vec2& out_uvAtMaxs ) const;
	//const SpriteSheet*	GetSpriteSheet() const { return m_spriteSheet; }
	//Texture*			GetTexture() const { return m_spriteSheet->GetTexture(); }
	float				GetAspect() const;

private:
// 	const SpriteSheet*		m_spriteSheet = nullptr;
// 	int						m_spriteIndex	= -1;
	IntVec2			m_sheetSize		= IntVec2::ZERO;
	Vec2			m_uvAtMins		= Vec2::ZERO;
	Vec2			m_uvAtMaxs		= Vec2::ONE;
};

