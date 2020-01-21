#pragma once
#include <string>
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/SpriteSheet.hpp"

struct Rgba8;

class BitmapFont {
	friend class RenderContext;

private:
	BitmapFont(const char* fontName, const Texture* fontTexture);

public:
	const Texture* GetTexture() const;

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertexArray, const Vec2& textMins, float cellHeight, const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f);
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertexArray, const AABB2& box, float cellHeight, const std::string& text, const Rgba8& tint = Rgba8::WHITE, float cellAspect = 1.f, const Vec2& alignment = ALIGN_CENTERED);
	Vec2 GetDimensionsForText2D( float cellHeight, const std::string& text, float cellAspect = 1.f);
	static Vec2 ALIGN_CENTERED;
protected:
	float GetGlyphAspect(int glyphUnicode) const;

protected:
	std::string m_fontName;
	SpriteSheet m_glyphSpriteSheet;
};