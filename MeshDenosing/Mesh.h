#pragma once
#include <iostream>
#include <vector> 
#include <map> 
#include <vector>
#include <string> 
#include <fstream> 
#include <sstream>
#include "Mymesh.h" 

using namespace std;

typedef std::pair<unsigned int, unsigned int> Pair;
typedef vector<Vertex>::iterator			v_iterator;
typedef vector<int>::iterator				i_iterator;
typedef vector<Face>::iterator				f_iterator;
typedef map<Pair, HalfEdge*>::iterator		map_iterator;

class MyMesh
{
public:
	vector<Vertex>       t_vertice;
	vector<Face>         t_face;
	map<Pair, HalfEdge*> hf_edge;
	MyMesh();
	MyMesh(const string file);
	void BuildMeshFromFile(const string filename);
	void SearchFace(int Vertex_ID);	
	vector<int> SearchAdjV(size_t  id);
	vector<Vertex> LaplacianSmoothing();
	vector<Vertex> HCLaplacianSmoothing(vector<Vertex> q);
	vector<Vertex> HC_LaplacianSmoothing(int times);
	void RestructMesh(vector<Vertex> smooth_Vertex);
	void DestoryMyMesh()
	{
		map_iterator i = hf_edge.begin();
		for (i; i != hf_edge.end(); i++)
		{
			delete i->second;
		}
	}
};

