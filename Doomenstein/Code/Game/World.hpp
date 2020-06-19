#pragma once
#include <vector>
#include <string>
#include "Engine/Core/StringUtils.hpp"

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
	Strings GetAllMaps() const;
	//void DestroyEntities();

private:

public:
	int	m_currentMapIndex = 0;
	std::vector<Map*> m_maps;

};


