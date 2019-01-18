#pragma once
#include <surface_mesh/Surface_mesh.h>
#include <Eigen/Dense>

using namespace std;
using namespace surface_mesh;

class MeshDenose
{
public:
	void getFaceArea(const Surface_mesh &mesh, vector<double> &area) const;
	void getFaceCentroid(const Surface_mesh &mesh, vector<Point> &centroid) const;
	void getFaceNormal(const Surface_mesh &mesh, vector<Normal> &normals) const;
	void getFaceFaceNeighbor(const Surface_mesh &mesh, Surface_mesh::Face f, vector<Surface_mesh::Face> &face_neighbor) const;
};