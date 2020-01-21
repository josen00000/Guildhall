#include "Map.hpp"
#include<engine/Math/vec2.hpp>
#include<Game/Player.hpp>
#include<engine/Core/Rgba8.hpp>
Map::Map(Game* iniGame, IntVec2 iniSize ,Vec2 iniPos)
	:m_game(iniGame)
	,m_size(iniSize)
	,m_pos(iniPos)
{
	m_tileNum=m_size.x*m_size.y;
}

int Map::GetTileIndexForTileCoords( const IntVec2& tileCoords ) const
{
	const int temIndex=tileCoords.x+(tileCoords.y*m_size.x);
	return temIndex;
}

IntVec2 Map::GetTileCoordsForTileIndex( const int tileIndex ) const
{
	int tileY=tileIndex/m_size.x;
	int tileX=tileIndex-(tileY*m_size.x);
	const IntVec2 temTileCoords=IntVec2(tileX,tileY);
	return temTileCoords;
}

void Map::SetPlayerPos(Player& inPlayer)
{
	m_selectingColor=inPlayer.m_color;
	IntVec2 playerPosForTileCoords=inPlayer.m_tilePos;
	int selectingTileIndex=GetTileIndexForTileCoords(playerPosForTileCoords);
	IntVec2 playerLastPosForTileCoords=inPlayer.m_lastTilePos;
	if(playerLastPosForTileCoords.x>=0){
		int backToNormalTileIndex=GetTileIndexForTileCoords(playerLastPosForTileCoords);
		m_tiles[backToNormalTileIndex].m_isSelecting=false;
	}
	m_tiles[selectingTileIndex].m_isSelecting=true;
	SetTileSelectingColor(selectingTileIndex);
		
}

void Map::Update()
{
	for( int tileIndex=0; tileIndex<m_tiles.size(); tileIndex++ ) {
		m_tiles[tileIndex].Update();
	}
}



bool Map::IsValidTileCoords( IntVec2 inTileCoords )
{
	IntVec2 temMapSize=m_size;
	if( inTileCoords.x>=0&&inTileCoords.x<=temMapSize.x-1&&inTileCoords.y>=0&&inTileCoords.y<=temMapSize.y-1 ) {
		return true;
	}
	else {
		return false;
	}
}

bool Map::IsValidTileIndex( int inIndex )
{
	if(inIndex>=0&&inIndex<=m_tileNum-1){
		return true;
	}
	else{
		return false;
	}

}

void Map::RenderTiles()
{
	for(int tileIndex=0;tileIndex<m_tiles.size();tileIndex++){
		m_tiles[tileIndex].RenderBase();
	}
	for( int tileIndex=0; tileIndex<m_tiles.size(); tileIndex++ ) {
		m_tiles[tileIndex].RenderSides();
	}
}

void Map::UpdateTiles()
{
	
}

void Map::Startup()
{
	CreateTiles();
}

void Map::SelectGridByPlayer( Player& inPlayer )
{
	IntVec2 playerPosForTileCoords=inPlayer.m_tilePos;
	int selectedTileIndex=GetTileIndexForTileCoords( playerPosForTileCoords );
	IntVec2 playerLastPosForTileCoords=inPlayer.m_lastTilePos;
	m_tiles[selectedTileIndex].m_isSelected=true;
	m_tiles[selectedTileIndex].m_spreadingColor=inPlayer.m_color;
	inPlayer.SelectTile(m_tiles[selectedTileIndex]);
}

void Map::SpreadGrids()
{
	for( int tileIndex=0; tileIndex<m_tiles.size(); tileIndex++ ) {
		if(m_tiles[tileIndex].m_isSpreading){
			Tile& temTile=m_tiles[tileIndex];
			temTile.Spread();
		
		}
	}
	

}

void Map::SpreadGrid(int inTileIndex)
{
	int leftIndex=inTileIndex-1;
	Rgba8 spreadColor=m_tiles[inTileIndex].m_spreadingColor;
	SpreadToNeighbour(leftIndex,1,spreadColor);
	int rightIndex=inTileIndex+1;
	SpreadToNeighbour(rightIndex,2,spreadColor);
	int upIndex=inTileIndex+m_size.y;
	SpreadToNeighbour(upIndex,0,spreadColor);
	int downIndex=inTileIndex-m_size.y;
	SpreadToNeighbour( downIndex,3 ,spreadColor);
	
	
}

void Map::SpreadToNeighbour(int inIndex,int triangleIndex,Rgba8 inSpreadColor)
{
	if( IsValidTileIndex( inIndex ) ) {
		Tile& neighbourTile=m_tiles[inIndex];
		neighbourTile.m_isSpreading=true;
		
		neighbourTile.m_isTriangleGetSpread[triangleIndex]=true;
		neighbourTile.m_spreadNum++;
		neighbourTile.m_spreadingColor=inSpreadColor;
	}
}

bool Map::CheckSuccess()
{
	for( int tileIndex=0; tileIndex<m_tileNum; tileIndex++ ) {
		if(!m_tiles[tileIndex].m_isSelected){
			return false;
		}
	}
	return true;
}

void Map::CreateTiles()
{// should be m_tileNum
	m_tiles.clear();
	for(int tileIndex=0;tileIndex<m_tileNum;tileIndex++){
		IntVec2 temTileCoords=GetTileCoordsForTileIndex(tileIndex);
		Tile newTile(m_game,this,m_pos,temTileCoords);
		m_tiles.push_back( newTile );
		m_tiles[tileIndex].Startup();
	}
}

void Map::SetTileSelectingColor(int tileIndex)
{
	m_tiles[tileIndex].m_selectingColor=m_selectingColor;
}

