#pragma once
#include "MeshDenose.h"
#include <vector>
#include <queue>
#include <set>

#include <surface_mesh/Surface_mesh.h>

using namespace std;
using namespace surface_mesh;

class NonIterativeFeaturePreservingMeshFiltering :public MeshDenose
{
public:
	void denoise(char** argv);
	void getFaceVertexNeighbor(const Surface_mesh & mesh, vector<Point>& face_centroid,
		Surface_mesh::Vertex v, double r, vector<Surface_mesh::Face> &vertex_face_neighbor) const;
	void mollifiedNormals(const Surface_mesh &mesh, vector<Point> &face_centroid, vector<double> &face_area, const double sigma_f, vector<Normal> &mollified_normals) const;
	Point projectPoint(Point pt, Point centroid, Normal n);

};