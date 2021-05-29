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
	const int& GetTotalMapIndex() const { return m_totalMapIndex; }
	const int& GetCurrentMapIndex() const { return m_currentMapIndex; }

	Map* GetCurrentMap();

	// Mutator
	void SetTotalMapIndex( int totalMapIndex );
	void SetCurrentMapIndex( int currentMapIndex );

private:
	int m_currentMapIndex = 0;
	int m_totalMapIndex = 0;
	std::vector<Map*> m_maps;
};