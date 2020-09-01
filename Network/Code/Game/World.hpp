#pragma once
#include <vector>
#include <string>
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EventSystem.hpp"

class Map;

class World
{
public:
	World();
	~World(){}
	void Update(float deltaSeconds);	
	void Render()const ;
	void AddMap( Map* map );
	bool LoadMap( std::string mapName );
	
	// accessor
	Strings GetAllMaps() const;
	Map* GetCurrentMap();
	//void DestroyEntities();

	// mutator
	void SetDebugMode( bool isDebug );
private:

public:
	bool m_isDebugMode = false;
	int	m_currentMapIndex = 0;
	std::vector<Map*> m_maps;

};

// Command
bool MapCommandRaycast( EventArgs& args );
