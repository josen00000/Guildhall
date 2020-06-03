#pragma once
#include "Engine/Core/StringUtils.hpp"

class InputSystem;
struct Vec2;

class Window
{
public:
	Window();
	~Window();
	
	bool Open( std::string const& title, float aspectRatio, float ratioOfHeight = 1.0f );
	void Close();
	void BeginFrame();
	
	// Accessor
	int GetClientWidth() const;
	int GetClientHeight() const;
	Vec2 GetClientCenter() const;
	void* GetHandle() const;

	void SetInputSystem( InputSystem* input );
	InputSystem* GetInputSystem() const { return m_inputSystem; }
	static void* GetTopWindowHandle();

public:
	void* m_hwnd = nullptr;
	bool m_isWindowClose = false;
	int m_clientWidth;
	int m_clientHeight;
	InputSystem* m_inputSystem = nullptr;
};




