#pragma once
#include <string>
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/Rgba8.hpp"

class Game;

class UIButton {
public:
	UIButton(){}
	~UIButton(){}
	UIButton( Vec2 spawnPos );

	static UIButton* CreateButtonWithPosSizeAndText( Vec2 pos, Vec2 size, std::string text );
	Vec2 GetPosition() const { return m_pos; }
	bool GetIsSelecting() const { return m_isSelecting; }


	// mutator
	void SetPosition( Vec2 pos );
	void SetDimension( Vec2 dimension );
	void SetDisplayText( std::string displayText );
	void SetEventName( std::string eventName );
	void SetTintColor( Rgba8 color );
	void SetTextColor( Rgba8 color );
	void SetSelectColor( Rgba8 color );
	void SetIsSelecting( bool isSelecting );

	void ExecuteTargetFunction();

	void RenderButton();
	void UpdateButton();

public:
	bool m_isSelecting	= false;
	Vec2 m_dimension	= Vec2::ZERO;
	Vec2 m_pos			= Vec2::ZERO;
	AABB2 m_shape		= AABB2( Vec2::ZERO, Vec2::ONE );
	Rgba8 m_tintColor	= Rgba8::BLUE;
	Rgba8 m_textColor	= Rgba8::BLACK;
	Rgba8 m_SelectColor	= Rgba8::RED;

	std::string m_text		= "";
	std::string m_eventName	= "";
	std::vector<Vertex_PCU> m_textVerts;
	EventArgs	m_eventArgs;
	EventCallbackFunctionPtr m_eventTarget;
public:
	static Rgba8 s_selectTextColor;
};
