#pragma once
#include <vector>
#include "Engine/Math/ConvexHull2.hpp"

template<class T>
struct SymmetricQuadTree {
	SymmetricQuadTree<T>* children[4]{nullptr}; // leftbottom, lefttop, rightbottom, righttop
	std::vector<T> objs;
	ConvexHull2 bound;

public:
	SymmetricQuadTree(){}
	explicit SymmetricQuadTree( SymmetricQuadTree* inputChildren, std::vector<T> content, ConvexHull2 hull );

	// Accessor
	bool IsNodeLeaf();
	ConvexHull2 GetConvexHull() const;
	void GetAllLeavesNode( std::vector<SymmetricQuadTree<T>*>& result );
	void GetAllIntersectLeavesNode( std::vector<SymmetricQuadTree<T>*>& result, Vec2 startPos, Vec2 fwdDirt, float dist );
	void GetAllIntersectLeavesNodeWithLine( std::vector<SymmetricQuadTree<T>*>& result, LineSegment2 line );
};

template<class T>
void SymmetricQuadTree<T>::GetAllIntersectLeavesNode( std::vector<SymmetricQuadTree<T>*>& result, Vec2 startPos, Vec2 fwdDirt, float dist )
{
	std::pair<Vec2, Plane2> possibleIntersectPointAndPlane = bound.GetMostPossibleIntersectPointAndPlaneWithRaycast( startPos, fwdDirt );
	float possibleDist = GetDistance2D( startPos, possibleIntersectPointAndPlane.first );
	if( possibleDist < dist && bound.IsPointInsideWithoutPlane( possibleIntersectPointAndPlane.first, possibleIntersectPointAndPlane.second ) ) {
		if( IsNodeLeaf() ) {
			result.push_back( this );
		}
		else {
			children[0]->GetAllIntersectLeavesNode( result, startPos, fwdDirt, dist );
			children[1]->GetAllIntersectLeavesNode( result, startPos, fwdDirt, dist );
			children[2]->GetAllIntersectLeavesNode( result, startPos, fwdDirt, dist );
			children[3]->GetAllIntersectLeavesNode( result, startPos, fwdDirt, dist );
		}
	}
	else if( IsNodeLeaf() ) {
		if( bound.IsPointInside( startPos ) ) {
			result.push_back( this );
		}
	}
}

template<class T>
ConvexHull2 SymmetricQuadTree<T>::GetConvexHull() const
{
	return bound;
}

template<class T>
bool SymmetricQuadTree<T>::IsNodeLeaf()
{
	return ( children[0] == nullptr );
}

template<class T>
void SymmetricQuadTree<T>::GetAllLeavesNode( std::vector<SymmetricQuadTree<T>*>& result )
{
	if( IsNodeLeaf() ) {
		result.push_back( this );
	}
	else {
		children[0]->GetAllLeavesNode( result );
		children[1]->GetAllLeavesNode( result );
		children[2]->GetAllLeavesNode( result );
		children[3]->GetAllLeavesNode( result );
	}
}

template<class T>
SymmetricQuadTree<T>::SymmetricQuadTree( SymmetricQuadTree* inputChildren, std::vector<T> content, ConvexHull2 hull )
{
	children = inputChildren;
	objs = content;
	bound = hull;
}
