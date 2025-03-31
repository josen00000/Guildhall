#pragma once
#include "RenderContext.hpp"
#include "Engine/Core/Time/Clock.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/SwapChain.hpp"

RenderContext* RenderContext::s_renderContext = nullptr;

RenderContext::RenderContext( RenderContextType type )
	:m_type( type )
{
	if( s_renderContext )
	{
		s_renderContext->ShutDown();
		delete s_renderContext;
	}
	
	s_renderContext = this;
	s_renderContext->m_clock = new Clock();
}


void RenderContext::SetModelMatrix( Mat44 model )
{
	m_model.modelMat = model;
	m_modelHasChanged = true;
}

Texture* RenderContext::GetSwapChainBackBuffer()
{
	return m_swapChain->GetBackBuffer();
}
