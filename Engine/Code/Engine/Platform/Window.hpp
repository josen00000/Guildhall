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

	void* m_hwnd;

};

