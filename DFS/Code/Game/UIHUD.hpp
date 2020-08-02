#pragma once
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

class UIHUD {
public:
	UIHUD(){}
	~UIHUD(){}
	UIHUD( Vec2 spawnPos );

	static UIHUD* CreateButtonWithPosSizeAndText( Vec2 pos, Vec2 size, std::string text );
	Vec2 GetPosition() const { return m_pos; }

	// mutator
	void SetPosition( Vec2 pos );
	void SetDimension( Vec2 dimension );
	void SetDisplayText( std::string displayText );
	void SetTintColor( Rgba8 color );
	void SetTextColor( Rgba8 color );


	void RenderButton();
	void UpdateButton();

public:
	Vec2 m_dimension	= Vec2::ZERO;
	Vec2 m_pos			= Vec2::ZERO;
	AABB2 m_shape		= AABB2( Vec2::ZERO, Vec2::ONE );
	Rgba8 m_tintColor	= Rgba8::BLUE;
	Rgba8 m_textColor	= Rgba8::BLACK;

	std::string m_text		= "";
	std::vector<Vertex_PCU> m_textVerts;
};
