#include"Tile.hpp"
#include<Engine/Math/Vec3.hpp>
#include<Game/Game.hpp>
#include<Engine/Renderer/RenderContext.hpp>
#include<game/Map.hpp>

Tile::Tile( const Tile& copyFrom )
	:m_game(copyFrom.m_game)
	,m_map(copyFrom.m_map)
	,m_centerPosInWorld(copyFrom.m_centerPosInWorld)
	,m_posInTileCoords(copyFrom.m_posInTileCoords)
{
	m_gridColor=m_map->m_baseColor;
	m_spreadingColor=m_map->m_baseColor;
	m_selectingColor=m_map->m_baseColor;
	m_sideColor=m_map->m_sideColor;
	m_baseColor=m_map->m_baseColor;
	for(int vertexIndex=0;vertexIndex<TILE_VERTEX_NUM;vertexIndex++){
		m_vertices[vertexIndex]=copyFrom.m_vertices[vertexIndex];
	}
}

Tile::Tile(Game* iniGame,Map* iniMap, Vec2& iniPosInWorld, IntVec2& iniPosInTileCoords )
	:m_game(iniGame)
	,m_map(iniMap)
	,m_centerPosInWorld( iniPosInWorld )
	,m_posInTileCoords( iniPosInTileCoords )
{
	m_spreadingColor=m_map->m_baseColor;
	m_selectingColor=m_map->m_baseColor;
	m_baseColor=m_map->m_baseColor;
	m_gridColor=m_map->m_baseColor;
	m_sideColor=m_map->m_sideColor;
}


void Tile::Createvertices()
{
	//trangle A down
	m_vertices[0]=Vertex_PCU( Vec3( 0.f, 0.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[1]=Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[2]=Vertex_PCU( Vec3( .5f, .5f, 0 ), m_gridColor, Vec2( 0, 0 ) ) ;
	//trangle B right
	m_vertices[3]=Vertex_PCU( Vec3( 1.f, 0.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[4]=Vertex_PCU( Vec3( 1.f, 1.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[5]=Vertex_PCU( Vec3( .5f, .5f, 0 ), m_gridColor, Vec2( 0, 0 ) ) ;
	//trangle C left
	m_vertices[6]=Vertex_PCU( Vec3( 0.f, 0.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[7]=Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[8]=Vertex_PCU( Vec3( .5f, .5f, 0 ), m_gridColor, Vec2( 0, 0 ) );
	//trangle D up
	m_vertices[9]=Vertex_PCU( Vec3( 0.f, 1.f, 0.f ), m_gridColor, Vec2( 0, 0 ));
	m_vertices[10]=Vertex_PCU( Vec3( 1.f, 1.f, 0.f ), m_gridColor, Vec2( 0, 0 ) );
	m_vertices[11]=Vertex_PCU( Vec3( .5f, .5f, 0 ), m_gridColor, Vec2( 0, 0 ) );
}

void Tile::Render()
{
	//render grid
	
	
	
}	

void Tile::Update()
{
	if(m_isSelecting){
		m_gridColor=m_selectingColor;
	}
	else if(m_isSelected){
		m_gridColor=m_spreadingColor;
	}
	else{
		m_gridColor=m_map->m_baseColor;
	
	}

}



void Tile::Startup()
{
	Createvertices();

	
}

Vec3 Tile::GetRenderPosition(Vec3 localPos)
{
	const Vec2 leftCornerPosition=GetWorldPosForTileCoords();
	Vec2 worldPos2D=TransformPosition2D(localPos,NORMAL_TILE_SCALE,NORMAL_TILE_ROTATION_DEGREE,leftCornerPosition);
	Vec3 worldPos3D=Vec3(worldPos2D,0);
	return worldPos3D;
}

Vec2 Tile::GetWorldPosForTileCoords()
{
	Vec2 worldPos=Vec2(m_posInTileCoords.x*NORMAL_TILE_SIDE_LENGTH,m_posInTileCoords.y*NORMAL_TILE_SIDE_LENGTH);
	worldPos+=m_centerPosInWorld;
	return worldPos;
}

void Tile::RenderBase()
{
	

	static Vertex_PCU temVertices[TILE_VERTEX_NUM]={};
	for(int vertexIndex=0;vertexIndex<TILE_VERTEX_NUM;vertexIndex++){
		Vec3 worldPos=GetRenderPosition(m_vertices[vertexIndex].m_pos);
		temVertices[vertexIndex]=(Vertex_PCU(worldPos,m_gridColor,Vec2(0,0)));
	}
	if(m_isSpreading){
		for(int triangleIndex=0;triangleIndex<4;triangleIndex++){
			if(m_isTriangleGetSpread[triangleIndex]){
				int startIndex=triangleIndex*3;
				for(int vertexIndex=startIndex;vertexIndex<startIndex+3;vertexIndex++){
					temVertices[vertexIndex].m_color=m_spreadingColor;
				}
			}
		}
	}
	m_game->m_theRenderer->DrawVertexArray(TILE_VERTEX_NUM,temVertices);
}

void Tile::RenderSides()
{
	static Vertex_PCU temSideVertices[]={
		Vertex_PCU( Vec3( 0.f, 0.f, 10.f ), m_sideColor, Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1.f, 0.f, 10.f ), m_sideColor, Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 1.f, 1.f, 10.f ), m_sideColor, Vec2( 0, 0 ) ),
		Vertex_PCU( Vec3( 0.f, 1.f, 10.f ), m_sideColor, Vec2( 0, 0 ) )
	};

	Vec3 worldPos=GetRenderPosition( m_vertices[0].m_pos );
	temSideVertices[0].m_pos=worldPos;
	worldPos=GetRenderPosition( m_vertices[1].m_pos );
	temSideVertices[1].m_pos=worldPos;
	worldPos=GetRenderPosition( m_vertices[4].m_pos );
	temSideVertices[2].m_pos=worldPos;
	worldPos=GetRenderPosition( m_vertices[7].m_pos );
	temSideVertices[3].m_pos=worldPos;

	m_game->m_theRenderer->DrawLine( temSideVertices[0].m_pos, temSideVertices[1].m_pos, m_sideThick, m_sideColor );
	m_game->m_theRenderer->DrawLine( temSideVertices[0].m_pos, temSideVertices[3].m_pos, m_sideThick, m_sideColor );
	m_game->m_theRenderer->DrawLine( temSideVertices[1].m_pos, temSideVertices[2].m_pos, m_sideThick, m_sideColor );
	m_game->m_theRenderer->DrawLine( temSideVertices[3].m_pos, temSideVertices[2].m_pos, m_sideThick, m_sideColor );
}

void Tile::Spread()
{
	if(m_spreadNum==0){
		return;
	}
	else if(m_spreadNum==1){
		if(m_isTriangleGetSpread[0]){
			m_isTriangleGetSpread[1]=true;
			m_isTriangleGetSpread[2]=true;
		}
		if( m_isTriangleGetSpread[1] ) {
			m_isTriangleGetSpread[0]=true;
			m_isTriangleGetSpread[3]=true;
		}
		if( m_isTriangleGetSpread[2] ) {
			m_isTriangleGetSpread[0]=true;
			m_isTriangleGetSpread[3]=true;
		}
		if( m_isTriangleGetSpread[3] ) {
			m_isTriangleGetSpread[1]=true;
			m_isTriangleGetSpread[2]=true;
		}

			m_spreadNum+=2;
	}
	else{
		m_isTriangleGetSpread[0]=true;
		m_isTriangleGetSpread[1]=true;
		m_isTriangleGetSpread[2]=true;
		m_isTriangleGetSpread[3]=true;
		m_spreadNum=3;
	}
}




bool Tile::IsSpreadComplete()
{
	for(int spreadIndex=0;spreadIndex<4;spreadIndex++){
		if(!m_isTriangleGetSpread[spreadIndex]){
			return false;
		}
		else{
			return true;
		}
	}
}
