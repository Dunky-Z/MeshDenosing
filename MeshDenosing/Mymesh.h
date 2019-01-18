#pragma once
struct HalfEdge;
struct Vertex;
struct Face;
struct HalfEdge
{
	HalfEdge* nextHalfEdge;
	HalfEdge* oppositeHalfEdge;
	Vertex*	  startPoint;
	Vertex*	  endPoint;
	Face*     face;
	bool      isBoundary;

};
struct Vertex
{
	HalfEdge*   halfEdge;
	long double x, y, z;
	int         idx;
};
struct Face
{
	HalfEdge*    halfEdge;
	int          idx;
	unsigned int v[3];
};

