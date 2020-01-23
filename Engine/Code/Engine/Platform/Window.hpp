#pragma once
#include "Engine/Core/StringUtils.hpp"


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

	void* m_hwnd;

public:
	int m_clientWidth;
	int m_clientHeight;
};




