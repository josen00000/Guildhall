#include "MeshUtils.hpp"
#include "Engine/core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/OBB3.hpp"


void AppendVertsForAABB2D( std::vector<Vertex_PCU>& vertices, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftdown		= Vertex_PCU( Vec3( bound.mins, -5.f ) , tintColor, uvAtMins );
	Vertex_PCU rightup		= Vertex_PCU( Vec3( bound.maxs, -5.f ), tintColor, uvAtMaxs );
	Vertex_PCU leftup		= Vertex_PCU( Vec3( bound.mins.x, bound.maxs.y, -5.f ), tintColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU rightdown	= Vertex_PCU( Vec3( bound.maxs.x, bound.mins.y, -5.f ), tintColor, Vec2( uvAtMaxs.x, uvAtMins.y ) );

	// left up triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( leftup );

	// right down triangle
	vertices.push_back( leftdown );
	vertices.push_back( rightup );
	vertices.push_back( rightdown );
}

void AppendVertsForAABB3D( std::vector<Vertex_PCU>& vertices, AABB3 box, Rgba8& tintColor )
{
	Vec3 frontLeftdownPos	= box.min;
	Vec3 frontRightupPos	= Vec3( box.max.x, box.max.y, box.min.z );
	Vec3 frontLeftupPos		= Vec3( box.min.x, box.max.y, box.min.z );
	Vec3 frontRightdownPos	= Vec3( box.max.x, box.min.y, box.min.z );

	Vec3 backLeftdownPos	= Vec3( box.max.x, box.min.y, box.max.z );
	Vec3 backRightupPos		= Vec3( box.min.x, box.max.y, box.max.z );
	Vec3 backLeftupPos		= box.max;
	Vec3 backRightdownPos	= Vec3( box.min.x, box.min.y, box.max.z );

	// front face
	Vertex_PCU frontLeftdown	= Vertex_PCU( frontLeftdownPos, tintColor, Vec2::ZERO );
	Vertex_PCU frontRightup		= Vertex_PCU( frontRightupPos, tintColor, Vec2::ONE );
	Vertex_PCU frontLeftup		= Vertex_PCU( frontLeftupPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU frontRightdown	= Vertex_PCU( frontRightdownPos, tintColor, Vec2( 1.f, 0.f ) );

	// back face
	Vertex_PCU backLeftdown		= Vertex_PCU( backLeftdownPos, tintColor, Vec2::ZERO );
	Vertex_PCU backRightup		= Vertex_PCU( backRightupPos, tintColor, Vec2::ONE );
	Vertex_PCU backLeftup		= Vertex_PCU( backLeftupPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU backRightdown	= Vertex_PCU( backRightdownPos, tintColor, Vec2( 1.f, 0.f ) );

	// left face
	Vertex_PCU leftLeftdown		= Vertex_PCU( backRightdownPos, tintColor, Vec2::ZERO );
	Vertex_PCU leftRightup		= Vertex_PCU( frontLeftupPos, tintColor, Vec2::ONE );
	Vertex_PCU leftLeftup		= Vertex_PCU( backRightupPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU leftRightdown	= Vertex_PCU( frontLeftdownPos, tintColor, Vec2( 1.f, 0.f ) );

	// right face
	Vertex_PCU rightLeftdown	= Vertex_PCU( frontRightdownPos, tintColor, Vec2::ZERO );
	Vertex_PCU rightRightup		= Vertex_PCU( backLeftupPos, tintColor, Vec2::ONE );
	Vertex_PCU rightLeftup		= Vertex_PCU( frontRightupPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU rightRightdown	= Vertex_PCU( backLeftdownPos, tintColor, Vec2( 1.f, 0.f ) );

	// top face
	Vertex_PCU topLeftdown		= Vertex_PCU( frontLeftupPos, tintColor, Vec2::ZERO );
	Vertex_PCU topRightup		= Vertex_PCU( backLeftupPos, tintColor, Vec2::ONE );
	Vertex_PCU topLeftup		= Vertex_PCU( backRightupPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU topRightdown		= Vertex_PCU( frontRightupPos, tintColor, Vec2( 1.f, 0.f ) );

	// bottom face
	Vertex_PCU bottomLeftdown	= Vertex_PCU( backRightdownPos, tintColor, Vec2::ZERO );
	Vertex_PCU bottomRightup	= Vertex_PCU( frontRightdownPos, tintColor, Vec2::ONE );
	Vertex_PCU bottomLeftup		= Vertex_PCU( frontLeftdownPos, tintColor, Vec2( 0.f, 1.f ) );
	Vertex_PCU bottomRightdown	= Vertex_PCU( backLeftdownPos, tintColor, Vec2( 1.f, 0.f ) );

	vertices.reserve( 36 );
	// front triangles
	vertices.push_back( frontLeftdown );
	vertices.push_back( frontRightup );
	vertices.push_back( frontRightdown );

	vertices.push_back( frontLeftdown );
	vertices.push_back( frontRightup );
	vertices.push_back( frontLeftup );

	// back triangles
	vertices.push_back( backLeftdown );
	vertices.push_back( backRightup );
	vertices.push_back( backRightdown );

	vertices.push_back( backLeftdown );
	vertices.push_back( backRightup );
	vertices.push_back( backLeftup );

	// left triangles
	vertices.push_back( leftLeftdown );
	vertices.push_back( leftRightup );
	vertices.push_back( leftRightdown );

	vertices.push_back( leftLeftdown );
	vertices.push_back( leftRightup );
	vertices.push_back( leftLeftup );

	// right triangles
	vertices.push_back( rightLeftdown );
	vertices.push_back( rightRightup );
	vertices.push_back( rightRightdown );

	vertices.push_back( rightLeftdown );
	vertices.push_back( rightRightup );
	vertices.push_back( rightLeftup );

	// top triangles
	vertices.push_back( topLeftdown );
	vertices.push_back( topRightup );
	vertices.push_back( topRightdown );

	vertices.push_back( topLeftdown );
	vertices.push_back( topRightup );
	vertices.push_back( topLeftup );

	// bottom triangles
	vertices.push_back( bottomLeftdown );
	vertices.push_back( bottomRightup );
	vertices.push_back( bottomRightdown );

	vertices.push_back( bottomLeftdown );
	vertices.push_back( bottomRightup );
	vertices.push_back( bottomLeftup );
}

void AppendVertsForCapsule2D( std::vector<Vertex_PCU>& vertices, const Capsule2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	UNUSED(uvAtMins);
	UNUSED(uvAtMaxs);
	OBB2 capsuleOBB = bound.GetCapsuleOBB2();
	AppendVertsForOBB2D( vertices, capsuleOBB, tintColor, Vec2::ZERO, Vec2::ZERO );
	Vec2 direction = bound.GetNormalizedDirection();
	float orientationDegrees = direction.GetAngleDegrees();
	orientationDegrees += 90;
	orientationDegrees = ClampDegressTo360( orientationDegrees );
	AppendVertsForHalfCircle2D( vertices, bound.m_start, orientationDegrees, bound.m_radius, tintColor );
	orientationDegrees += 180;
	orientationDegrees = ClampDegressTo360( orientationDegrees );
	AppendVertsForHalfCircle2D( vertices, bound.m_end, orientationDegrees, bound.m_radius, tintColor );

}

void AppendVertsForOBB2D( std::vector<Vertex_PCU>& vertices, const OBB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	Vertex_PCU leftDown = Vertex_PCU( bound.GetLeftDownCornerForIBasis(), tintColor, uvAtMins );
	Vertex_PCU leftUp = Vertex_PCU( bound.GetLeftUpCornerForIBasis(), tintColor, Vec2( uvAtMins.x, uvAtMaxs.y ) );
	Vertex_PCU rightDown = Vertex_PCU( bound.GetRightDownCornerForIBasis(), tintColor, Vec2( uvAtMaxs.x, uvAtMins.y ) );
	Vertex_PCU rightUp = Vertex_PCU( bound.GetrightUpCornerForIBasis(), tintColor, uvAtMaxs );

	//triangle 1
	vertices.push_back( leftDown );
	vertices.push_back( leftUp );
	vertices.push_back( rightUp );
	//triangle 2
	vertices.push_back( leftDown );
	vertices.push_back( rightDown );
	vertices.push_back( rightUp );
}

void AppendVertsForOBB3D( std::vector<Vertex_PCU>& vertices, const OBB3& bound, const Rgba8& tintColor )
{

}

void AppendVertsForCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float radius, const Rgba8& tintColor )
{
	int triangleNumber = 64;
	float deltaDegrees = ( (float)360 / triangleNumber );
	float tempDegrees = 0; // current degrees
	float nextDegrees;
	Vec2 tempDegreesDirection = Vec2();
	Vec2 nextDegreesDirection = Vec2();
	Vertex_PCU centerVertex = Vertex_PCU( Vec3( center ), tintColor, Vec2::ZERO );

	for(int triangleIndex = 0; triangleIndex < triangleNumber; ++triangleIndex ){
		nextDegrees = tempDegrees + deltaDegrees;
		tempDegreesDirection.SetPolarDegrees( tempDegrees, 1 );
		nextDegreesDirection.SetPolarDegrees( nextDegrees, 1 );
		Vec2 tempDegreesPoint = center + tempDegreesDirection * radius;
		Vec2 nextDegreesPoint = center + nextDegreesDirection * radius;
		Vertex_PCU tempDegreesVertex = Vertex_PCU( Vec3( tempDegreesPoint ), tintColor, Vec2::ZERO );
		Vertex_PCU nextDegreesVertex = Vertex_PCU( Vec3( nextDegreesPoint ), tintColor, Vec2::ZERO );
		vertices.push_back( tempDegreesVertex );
		vertices.push_back( nextDegreesVertex );
		vertices.push_back( centerVertex );
		tempDegrees = nextDegrees;
	}
}

void AppendVertsForHalfCircle2D( std::vector<Vertex_PCU>& vertices, const Vec2& center, float orientationDegrees, float radius, const Rgba8& tintColor )
{
	int triangleNumber = 11;
	float deltaDegrees = ( (float)180 / triangleNumber );
	float tempDegrees = orientationDegrees; // current degrees
	float nextDegrees;
	Vec2 tempDegreesDirection = Vec2();
	Vec2 nextDegreesDirection = Vec2();
	Vertex_PCU centerVertex = Vertex_PCU( Vec3( center ), tintColor, Vec2::ZERO );
	//triangleNumber+1
	for( int triangleIndex = 0; triangleIndex < triangleNumber; ++triangleIndex ) {
		nextDegrees = tempDegrees + deltaDegrees;
		tempDegreesDirection.SetPolarDegrees( tempDegrees, 1 );
		nextDegreesDirection.SetPolarDegrees( nextDegrees, 1 );
		Vec2 tempDegreesPoint = center + tempDegreesDirection * radius;
		Vec2 nextDegreesPoint = center + nextDegreesDirection * radius;
		Vertex_PCU tempDegreesVertex = Vertex_PCU( Vec3( tempDegreesPoint ), tintColor, Vec2::ZERO );
		Vertex_PCU nextDegreesVertex = Vertex_PCU( Vec3( nextDegreesPoint ), tintColor, Vec2::ZERO );
		vertices.push_back( tempDegreesVertex );
		vertices.push_back( nextDegreesVertex );
		vertices.push_back( centerVertex );
		tempDegrees = nextDegrees;
	}
}

void AppendVertsForSphere3D( std::vector<Vertex_PCU>& vertices, Vec3 center, float radius, int hCut, int vCut, Rgba8& tintColor )
{
	float deltaTheta = 360.f / (float)vCut;
	float deltaPhi	 = 180.f / (float)hCut;
	for( int rowIndex = 0; rowIndex < hCut; rowIndex++ ){
		float phi = -90.f + rowIndex * deltaPhi;
		for( int columnIndex = 0; columnIndex < vCut; columnIndex++ ){
			// append face
			float theta = columnIndex * deltaTheta;
			Vec3 posLeftDown	= Vec3::MakeFromPolarDegrees( theta, phi, radius );
			Vec3 posRightDown	= Vec3::MakeFromPolarDegrees( ( theta + deltaTheta ), phi, radius );
			Vec3 posLeftUp		= Vec3::MakeFromPolarDegrees( theta, ( phi + deltaPhi ), radius );	
			Vec3 posRightUp		= Vec3::MakeFromPolarDegrees( ( theta + deltaTheta ), ( phi + deltaPhi ), radius );

			Vec2 uvLeftDown		= MakeSphereUVFromPolayDegrees( theta, phi );
			Vec2 uvRightDown	= MakeSphereUVFromPolayDegrees( ( theta + deltaTheta ), phi );
			Vec2 uvLeftUp		= MakeSphereUVFromPolayDegrees( theta, ( phi + deltaPhi ) );
			Vec2 uvRightup		= MakeSphereUVFromPolayDegrees( ( theta + deltaTheta ), ( phi + deltaPhi ) );

			vertices.push_back( Vertex_PCU( ( posLeftDown + center ),	tintColor, uvLeftDown ) );
			vertices.push_back( Vertex_PCU( ( posRightDown + center ),	tintColor, uvRightDown ) );
			vertices.push_back( Vertex_PCU( ( posRightUp + center ),	tintColor, uvRightup ) );

			vertices.push_back( Vertex_PCU( ( posLeftDown + center ),	tintColor, uvLeftDown ) );
			vertices.push_back( Vertex_PCU( ( posLeftUp + center ),		tintColor, uvLeftUp ) );
			vertices.push_back( Vertex_PCU( ( posRightUp + center ),	tintColor, uvRightup ) );
		}
	}

}

void AppendVertsForCubeSPhere3D( std::vector<Vertex_PCU>& vertices, const AABB3& cube, int level )
{
	// TODO unfinished
	// assume cube be cube.
	GUARANTEE_OR_DIE( cube.IsCube(), "input box is not a cube!" );
	
	std::vector<Vertex_PCU> cubeVertices;
	AppendVertsForAABB3D( cubeVertices, cube, Rgba8::WHITE );

	float halfSideLength = cube.GetSideLength() / 2;
	Vec3 offset =  Vec3( halfSideLength );

	int verticesNumber = 36 * ( level * level );
	vertices.reserve( verticesNumber );
	
	// front, back, left, right
	for( int faceIndex = 0; faceIndex < 4; faceIndex++ ) {
		Vertex_PCU leftDown		= cubeVertices[faceIndex * 6];
		Vertex_PCU rightUp		= cubeVertices[(faceIndex * 6) + 1];
		Vertex_PCU rightdown	= cubeVertices[(faceIndex * 6) + 2];
		Vertex_PCU leftUp		= cubeVertices[(faceIndex * 6) + 5];

		leftDown.m_pos	-= offset;
		rightUp.m_pos	-= offset;
		rightdown.m_pos -= offset;
		leftUp.m_pos	-= offset;

		// pos
		float leftDownTheta	= leftDown.m_pos.GetThetaDegrees();
		float rightUpTheta	= rightUp.m_pos.GetThetaDegrees();
		float leftDownPhi	= leftDown.m_pos.GetPhiDegrees();
		float rightUpPhi	= rightUp.m_pos.GetPhiDegrees();

		float deltaTheta	= (rightUpTheta - leftDownTheta) / level;
		float deltaPhi		= (rightUpPhi - leftDownPhi) / level;

		// uv
		float deltaU	= ( rightUp.m_uvTexCoords.x - leftDown.m_uvTexCoords.x ) / level;
		float deltaV	= ( rightUp.m_uvTexCoords.y - leftDown.m_uvTexCoords.y ) / level;

		float diagonalLength = cube.GetDiagonalsLength();
		diagonalLength *= 0.5f;

		for( int i = 0; i < level; i++ ) {
			float phi	= leftDownPhi + deltaPhi * i;
			float v		= leftDown.m_uvTexCoords.y + deltaV * i;
			for( int j = 0; j < level; j++ ) {
				float theta = leftDownTheta + deltaTheta * i;
				float u		= leftDown.m_uvTexCoords.x + deltaU * i;

				Vec3 tinyLeftDownPos	= Vec3::MakeFromPolarDegrees( theta, phi, diagonalLength ); 
				Vec3 tinyRightUpPos		= Vec3::MakeFromPolarDegrees( ( theta + deltaTheta ), ( phi + deltaPhi ), diagonalLength );
				Vec3 tinyLeftUpPos		= Vec3::MakeFromPolarDegrees( theta, ( phi + deltaPhi ), diagonalLength );
				Vec3 tinyRightDownPos	= Vec3::MakeFromPolarDegrees( ( theta + deltaTheta ), phi, diagonalLength );

				Vec2 tinyLeftDownUV		= Vec2( u, v );
				Vec2 tinyRightUpUV		= Vec2( ( u + deltaU ), ( v + deltaV ) );
				Vec2 tinyLeftUpUV		= Vec2( u, ( v + deltaV ) );
				Vec2 tinyRightDownUV	= Vec2( ( u + deltaU ), v );

				Vertex_PCU tinyLeftDown = Vertex_PCU( tinyLeftDownPos, Rgba8::WHITE, tinyLeftDownUV );
				Vertex_PCU tinyRightUp	= Vertex_PCU( tinyRightUpPos, Rgba8::WHITE, tinyRightUpUV );
				Vertex_PCU tinyLeftUp	= Vertex_PCU( tinyLeftUpPos, Rgba8::WHITE, tinyLeftUpUV );
				Vertex_PCU tinyRightDown	= Vertex_PCU( tinyRightDownPos, Rgba8::WHITE, tinyRightDownUV );

				vertices.push_back( tinyLeftDown );
				vertices.push_back( tinyRightUp );
				vertices.push_back( tinyLeftUp );

				vertices.push_back( tinyLeftDown );
				vertices.push_back( tinyRightUp );
				vertices.push_back( tinyRightDown );
			}

		}
	}

	// top and down face
	for( int faceIndex = 4; faceIndex < 6; faceIndex++ ) {
		Vertex_PCU leftDown		= cubeVertices[faceIndex * 6];
		Vertex_PCU rightUp		= cubeVertices[(faceIndex * 6) + 1];
		Vertex_PCU rightdown	= cubeVertices[(faceIndex * 6) + 2];
		Vertex_PCU leftUp		= cubeVertices[(faceIndex * 6) + 5];

		leftDown.m_pos	-= offset;
		rightUp.m_pos	-= offset;
		rightdown.m_pos -= offset;
		leftUp.m_pos	-= offset;

		// pos
		float leftDownTheta	= leftDown.m_pos.GetThetaDegrees();
		float rightUpTheta	= rightUp.m_pos.GetThetaDegrees();
		float leftDownPhi	= leftDown.m_pos.GetPhiDegrees();
		float rightUpPhi	= rightUp.m_pos.GetPhiDegrees();

		float deltaTheta	= (rightUpTheta - leftDownTheta) / level;
		float deltaPhi		= (rightUpPhi - leftDownPhi) / level;

		// uv
		float deltaU	= (rightUp.m_uvTexCoords.x - leftDown.m_uvTexCoords.x) / level;
		float deltaV	= (rightUp.m_uvTexCoords.y - leftDown.m_uvTexCoords.y) / level;

		float diagonalLength = cube.GetDiagonalsLength();
		diagonalLength *= 0.5f;

		for( int i = 0; i < level; i++ ) {
			float phi	= leftDownPhi + deltaPhi * i;
			float v		= leftDown.m_uvTexCoords.y + deltaV * i;
			for( int j = 0; j < level; j++ ) {
				float theta = leftDownTheta + deltaTheta * i;
				float u		= leftDown.m_uvTexCoords.x + deltaU * i;

				Vec3 tinyLeftDownPos	= Vec3::MakeFromPolarDegrees( theta, phi, diagonalLength );
				Vec3 tinyRightUpPos		= Vec3::MakeFromPolarDegrees( (theta + deltaTheta), (phi + deltaPhi), diagonalLength );
				Vec3 tinyLeftUpPos		= Vec3::MakeFromPolarDegrees( theta, (phi + deltaPhi), diagonalLength );
				Vec3 tinyRightDownPos	= Vec3::MakeFromPolarDegrees( (theta + deltaTheta), phi, diagonalLength );

				Vec2 tinyLeftDownUV		= Vec2( u, v );
				Vec2 tinyRightUpUV		= Vec2( (u + deltaU), (v + deltaV) );
				Vec2 tinyLeftUpUV		= Vec2( u, (v + deltaV) );
				Vec2 tinyRightDownUV	= Vec2( (u + deltaU), v );

				Vertex_PCU tinyLeftDown = Vertex_PCU( tinyLeftDownPos, Rgba8::WHITE, tinyLeftDownUV );
				Vertex_PCU tinyRightUp	= Vertex_PCU( tinyRightUpPos, Rgba8::WHITE, tinyRightUpUV );
				Vertex_PCU tinyLeftUp	= Vertex_PCU( tinyLeftUpPos, Rgba8::WHITE, tinyLeftUpUV );
				Vertex_PCU tinyRightDown	= Vertex_PCU( tinyRightDownPos, Rgba8::WHITE, tinyRightDownUV );

				vertices.push_back( tinyLeftDown );
				vertices.push_back( tinyRightUp );
				vertices.push_back( tinyLeftUp );

				vertices.push_back( tinyLeftDown );
				vertices.push_back( tinyRightUp );
				vertices.push_back( tinyRightDown );
			}

		}
	}
}

void AppendVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, Cylinder3 cylinder, int level, Rgba8& tintColor )
{
	Mat44 lookatMat = Mat44::GetLookAtMatrix( cylinder.GetStartPos(), cylinder.GetEndPos() );

	float thetaDeg = 0.f;
	float deltaThetaDeg = 360.f / level; 
	
	Vertex_PCU start	= Vertex_PCU( cylinder.GetStartPos(), tintColor, Vec2::ZERO );
	Vertex_PCU end		= Vertex_PCU( cylinder.GetEndPos(), tintColor, Vec2::ZERO );
	for( int i = 0; i < level; i++ ){
		float cosThetaDeg = CosDegrees( thetaDeg );
		float sinThetaDeg = SinDegrees( thetaDeg );
		float cosNextThetaDeg = CosDegrees( thetaDeg + deltaThetaDeg );
		float sinNextThetaDeg = SinDegrees( thetaDeg + deltaThetaDeg );
		
		Vertex_PCU startCurrent	= Vertex_PCU( cylinder.GetStartPos() + ( cosThetaDeg * lookatMat.GetIBasis3D() ) + ( sinThetaDeg * lookatMat.GetJBasis3D() ), tintColor, Vec2::ZERO );
		Vertex_PCU startNext	= Vertex_PCU( cylinder.GetStartPos() + ( cosNextThetaDeg * lookatMat.GetIBasis3D() ) + ( sinNextThetaDeg * lookatMat.GetJBasis3D() ), tintColor, Vec2::ZERO );
		Vertex_PCU endCurrent	= Vertex_PCU( cylinder.GetEndPos() + (cosThetaDeg * lookatMat.GetIBasis3D()) + (sinThetaDeg * lookatMat.GetJBasis3D()), tintColor, Vec2::ZERO );
		Vertex_PCU endNext		= Vertex_PCU( cylinder.GetEndPos() + (cosNextThetaDeg * lookatMat.GetIBasis3D()) + (sinNextThetaDeg * lookatMat.GetJBasis3D()), tintColor, Vec2::ZERO );

		// start disc
		vertices.push_back( start );
		vertices.push_back( startCurrent );
		vertices.push_back( startNext );

		/// end disc
		vertices.push_back( end );
		vertices.push_back( endCurrent );
		vertices.push_back( endNext );

		// side
		vertices.push_back( startCurrent );
		vertices.push_back( endCurrent );
		vertices.push_back(startNext );
		
		vertices.push_back( startNext );
		vertices.push_back( endCurrent );
		vertices.push_back( endNext );

		thetaDeg = thetaDeg + deltaThetaDeg;
	}
}

void AppendVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, Cylinder3 cylinder, int level, Rgba8& startTintColor, Rgba8& endTintColor )
{
	Mat44 lookatMat = Mat44::GetLookAtMatrix( cylinder.GetStartPos(), cylinder.GetEndPos() );

	float thetaDeg = 0.f;
	float deltaThetaDeg = 360.f / level;

	float radius = cylinder.GetRadius();

	Vertex_PCU start	= Vertex_PCU( cylinder.GetStartPos(), startTintColor, Vec2::ZERO );
	Vertex_PCU end		= Vertex_PCU( cylinder.GetEndPos(), endTintColor, Vec2::ZERO );
	for( int i = 0; i < level; i++ ) {
		float cosThetaDeg = CosDegrees( thetaDeg );
		float sinThetaDeg = SinDegrees( thetaDeg );
		float cosNextThetaDeg = CosDegrees( thetaDeg + deltaThetaDeg );
		float sinNextThetaDeg = SinDegrees( thetaDeg + deltaThetaDeg );

		Vertex_PCU startCurrent	= Vertex_PCU( cylinder.GetStartPos() + (cosThetaDeg * lookatMat.GetIBasis3D() * radius ) + (sinThetaDeg * lookatMat.GetJBasis3D() * radius ), startTintColor, Vec2::ZERO );
		Vertex_PCU startNext	= Vertex_PCU( cylinder.GetStartPos() + (cosNextThetaDeg * lookatMat.GetIBasis3D() * radius ) + (sinNextThetaDeg * lookatMat.GetJBasis3D() * radius ), startTintColor, Vec2::ZERO );
		Vertex_PCU endCurrent	= Vertex_PCU( cylinder.GetEndPos() + (cosThetaDeg * lookatMat.GetIBasis3D() * radius ) + (sinThetaDeg * lookatMat.GetJBasis3D() * radius ), endTintColor, Vec2::ZERO );
		Vertex_PCU endNext		= Vertex_PCU( cylinder.GetEndPos() + (cosNextThetaDeg * lookatMat.GetIBasis3D() * radius ) + (sinNextThetaDeg * lookatMat.GetJBasis3D() * radius ), endTintColor, Vec2::ZERO );

		 //start disc
		vertices.push_back( start );
		vertices.push_back( startCurrent );
		vertices.push_back( startNext );
		
		// end disc
		vertices.push_back( end );
		vertices.push_back( endCurrent );
		vertices.push_back( endNext );

		// side
 		vertices.push_back( startCurrent );
 		vertices.push_back( endCurrent );
 		vertices.push_back( startNext );
 
 		vertices.push_back( startNext );
 		vertices.push_back( endCurrent );
 		vertices.push_back( endNext );

		thetaDeg = thetaDeg + deltaThetaDeg;
	}
}

void AppendVertsForCone3D( std::vector<Vertex_PCU>& vertices, Cone3 cone, int level, Rgba8& tintColor )
{
	Mat44 lookatMat = Mat44::GetLookAtMatrix( cone.GetStartPos(), cone.GetEndPos() );

	float thetaDeg = 0.f;
	float deltaThetaDeg = 360.f / level;
	float radius = cone.GetRadius();

	Vertex_PCU start	= Vertex_PCU( cone.GetStartPos(), tintColor, Vec2::ZERO );
	Vertex_PCU end		= Vertex_PCU( cone.GetEndPos(), tintColor, Vec2::ZERO );
	for( int i = 0; i < level; i++ ) {
		float cosThetaDeg = CosDegrees( thetaDeg );
		float sinThetaDeg = SinDegrees( thetaDeg );
		float cosNextThetaDeg = CosDegrees( thetaDeg + deltaThetaDeg );
		float sinNextThetaDeg = SinDegrees( thetaDeg + deltaThetaDeg );

		Vertex_PCU startCurrent	= Vertex_PCU( cone.GetStartPos() + ( cosThetaDeg * lookatMat.GetIBasis3D() * radius ) + ( sinThetaDeg * lookatMat.GetJBasis3D() * radius ), tintColor, Vec2::ZERO );
		Vertex_PCU startNext	= Vertex_PCU( cone.GetStartPos() + ( cosNextThetaDeg * lookatMat.GetIBasis3D() * radius ) + ( sinNextThetaDeg * lookatMat.GetJBasis3D() * radius ), tintColor, Vec2::ZERO );

		// start disc
		vertices.push_back( start );
		vertices.push_back( startCurrent );
		vertices.push_back( startNext );

		// side
		vertices.push_back( startCurrent );
		vertices.push_back( end );
		vertices.push_back( startNext );

		thetaDeg = thetaDeg + deltaThetaDeg;
	}
}

void AppendIndexedVerts( std::vector<Vertex_PCU>& dest, std::vector<uint>& index, const std::vector<Vertex_PCU> source )
{
	for( int sIndex = 0; sIndex < source.size(); sIndex++ ){
		const Vertex_PCU& sourceVertex = source[sIndex];
		uint pointIndex;
		if( IsPointInsideDest( sourceVertex, dest, pointIndex ) ){
			index.push_back( pointIndex );
		}
		else{
			dest.push_back( sourceVertex );
			index.push_back( (uint)dest.size() - 1 );
		}
	}
}

void AppendIndexedVertsForAABB2D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const AABB2& bound, const Rgba8& tintColor, const Vec2& uvAtMins, const Vec2& uvAtMaxs )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForAABB2D( verticesNotIndexed, bound, tintColor, uvAtMins, uvAtMaxs );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForSphere3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Vec3 center, float radius,  int hCut, int vCut, Rgba8& tintColor )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForSphere3D( verticesNotIndexed, center, radius, hCut, vCut, tintColor );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForAABB3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, AABB3 box, Rgba8& tintColor )
{
	std::vector<Vertex_PCU>verticesNotIndexed;
	AppendVertsForAABB3D( verticesNotIndexed, box, tintColor );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForCubeSphere3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, const AABB3& cube, int level )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForCubeSPhere3D( verticesNotIndexed, cube, level );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cylinder3 cylinder, int level, Rgba8& tintColor )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForCylinder3D( verticesNotIndexed, cylinder, level, tintColor );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForCylinder3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cylinder3 cylinder, int level, Rgba8& startTintColor, Rgba8& endTintColor )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForCylinder3D( verticesNotIndexed, cylinder, level, startTintColor, endTintColor );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

void AppendIndexedVertsForCone3D( std::vector<Vertex_PCU>& vertices, std::vector<uint>& index, Cone3 cone, int level, Rgba8& tintColor )
{
	std::vector<Vertex_PCU> verticesNotIndexed;
	AppendVertsForCone3D( verticesNotIndexed, cone, level, tintColor );
	AppendIndexedVerts( vertices, index, verticesNotIndexed );
}

bool IsPointInsideDest( const Vertex_PCU point, const std::vector<Vertex_PCU> dest, uint& index ) 
{
	if( dest.size() == 0 ){ return false; }

	for( uint dIndex = 0; dIndex < dest.size(); dIndex++ ){
		if( point == dest[dIndex] ){
			index = dIndex;
			return true;
		}
	}
	return false;
}

Vec2 MakeSphereUVFromPolayDegrees( float theta, float phi )
{
	Vec2 result;
	result.x = RangeMapFloat( 0.f, 360.f, 0.f, 1.f, theta );
	result.y = RangeMapFloat( -90.f, 90.f, 0.f, 1.f, phi );
	return result;
}

void AppendTesselateIndexedVerts( std::vector<Vertex_PCU>& dest, std::vector<uint>& index, const std::vector<Vertex_PCU> source )
{
	// For Each triangle in dest
	// find total 6 vertices
	// append 4 triangle into the dest
	std::vector<Vertex_PCU> vertices;
	GUARANTEE_OR_DIE( source.size() % 3 == 0, std::string( "verts are not triangles!" ) );
	int  i = 0;
	while( i < source.size() - 2  ){
		Vertex_PCU vertA = source[i];
		Vertex_PCU vertB = source[i + 1];
		Vertex_PCU vertC = source[i + 2];
		Vertex_PCU vertAB = Vertex_PCU::GetMiddleVertForTwoVerts( vertA, vertB );
		Vertex_PCU vertBC = Vertex_PCU::GetMiddleVertForTwoVerts( vertB, vertC );
		Vertex_PCU vertCA = Vertex_PCU::GetMiddleVertForTwoVerts( vertC, vertA );

		//triangle A, AB, AC
		vertices.push_back( vertA );
		vertices.push_back( vertAB );
		vertices.push_back( vertCA );

		//triangle AB, B, BC
		vertices.push_back( vertAB );
		vertices.push_back( vertB );
		vertices.push_back( vertBC );
		
		// triangle AB, BC, AC
		vertices.push_back( vertAB );
		vertices.push_back( vertBC );
		vertices.push_back( vertCA );
		
		// triangle AC, BC, CC
		vertices.push_back( vertCA );
		vertices.push_back( vertBC );
		vertices.push_back( vertC );
		i = i + 3;
	}

	AppendIndexedVerts( dest, index, vertices );
}

