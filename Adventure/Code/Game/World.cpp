#include "World.hpp"
#include "Game/Map.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Input/InputSystem.hpp"

extern RenderContext* g_theRenderer;
extern Camera* g_UICamera;
extern InputSystem* g_theInputSystem;
extern Game* g_theGame;
extern BitmapFont* g_squirrelFont;

World::World()
{
	m_maps[0] = new Map( this, "Island", &MapDefinition::s_definitions["Island"] );
	m_mapCreateIndex++;
	m_currentMap = m_maps[m_mapIndex];
}

void World::Update( float deltaSeconds )
{
	m_currentMap->Update( deltaSeconds );
}

void World::Render()const
{
	if(!m_isRenderGame) {return;}
	m_currentMap->Render();
}

void World::DestroyEntities()
{
	m_currentMap->DestroyEntities();
}













