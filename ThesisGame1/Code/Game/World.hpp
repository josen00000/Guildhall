#pragma once
#include <vector>
#include "Game/Game.hpp"
#include "Game/Map/Map.hpp"

class World {
public:
	World(){}
	~World();

public:
	static World* CreateWorld( int totalMapIndex );

public:
	void CreateMaps();
	void UpdateWorld( float deltaSeconds );
	void RenderWorld() const;
	void EndFrame();

	// Accessor
	int GetTotalMapIndex() const { return m_totalMapIndex; }
	Map* GetCurrentMap();

	// Mutator
	void SetTotalMapIndex( int totalMapIndex );

private:
	int m_currentMapIndex = 0;
	int m_totalMapIndex = 0;
	std::vector<Map*> m_maps;
};