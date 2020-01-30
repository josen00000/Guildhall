#pragma once
#include "Engine/Core/StringUtils.hpp"

class InputSystem;

class Window
{
public:
	Window();
	~Window();
	
	bool Open( std::string const& title, float aspectRatio, float ratioOfHeight = 0.90f );
	void Close();
	void BeginFrame();
	int GetClientWidth() const;
	int GetClientHeight() const;

	void SetInputSystem( InputSystem* input );
	InputSystem* GetInputSystem() const { return m_inputSystem; }

public:
	void* m_hwnd = nullptr;
	bool m_isWindowClose = false;
	int m_clientWidth;
	int m_clientHeight;
	InputSystem* m_inputSystem = nullptr;
};




