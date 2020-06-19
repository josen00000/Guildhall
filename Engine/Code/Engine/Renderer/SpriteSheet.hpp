#pragma once
#include <vector>
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/math/IntVec2.hpp"

class Texture;
struct IntVec2;
struct Vec2;


class SpriteSheet
{
public:
	explicit SpriteSheet( Texture& texture, const IntVec2& simpleGridLayout);
	Texture&			GetTexture() const		{ return m_texture; }
	int						GetNumSprites() const	{return (int) m_spriteDefs.size();}
	const SpriteDefinition& GetSpriteDefinition(int spriteIndex) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, int spriteIndex) const;
	void					GetSpriteUVs(Vec2& out_uvAtMins, Vec2& out_uvAtMaxs, IntVec2 spriteCoords ) const;
private:
	void CreateSpriteDefinitions(const IntVec2 simpleGridLayout);
protected:
	Texture&						m_texture;
	std::vector<SpriteDefinition>	m_spriteDefs;
	IntVec2							m_layout;
};

