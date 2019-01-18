#include "stdafx.h"
#include "MeshDenose.h"
#include <surface_mesh/Surface_mesh.h>
#include <vector>
#include <set>


using namespace std;
using namespace surface_mesh;
using namespace Eigen;

void MeshDenose::getFaceArea(const Surface_mesh &mesh, vector<double> &area) const
{
	area.resize(mesh.n_faces());
	Surface_mesh::Vertex_property<Point> pts = mesh.get_vertex_property<Point>("v:point");
	for (auto f_it : mesh.faces())
	{
		Point pt(0, 0, 0);
		int i = 0;
		Matrix3f  V;
		Surface_mesh::Face_iterator f = mesh.faces_begin();
		Surface_mesh::Vertex_around_face_circulator vf, vf_end;
		vf = mesh.vertices(f_it);
		vf_end = vf;
		do {
			Point pt = pts[*vf];
			V.col(i++) = Eigen::Map<const Eigen::Vector3f>(pt.data());
		} while (++vf != vf_end);
		Vector3f edge1 = V.col(1) - V.col(0);
		Vector3f edge2 = V.col(1) - V.col(2);
		double S = 0.5 *((edge1.cross(edge2)).norm());
		area[f_it.idx_] = S;
		/*
		for (auto vf_it : mesh.vertices())
		{
			pt = Surface_mesh::points[vf_it];
			V.col(i++) = Eigen::Map<const Eigen::Vector3f>(pt.data());
		}
		Vector3f edge1 = V.col(1) - V.col(0);
		Vector3f edge2 = V.col(1) - V.col(2);
		double S = 0.5 *((edge1.cross(edge2)).norm());
		area[f_it.idx_] = S;
		*/
	}
}

void MeshDenose::getFaceCentroid(const Surface_mesh &mesh, vector<Point> &centroid) const
{
	centroid.resize(mesh.n_faces(), Point(0.0, 0.0, 0.0));
	Surface_mesh::Vertex_property<Point> pts = mesh.get_vertex_property<Point>("v:point");

	for (auto f_it : mesh.faces())
	{
		Surface_mesh::Face_iterator f = mesh.faces_begin();
		Surface_mesh::Vertex_around_face_circulator vf, vf_end;
		vf = mesh.vertices(f_it);
		vf_end = vf;
		Point pt(0, 0, 0);
		int i = 0;
		Eigen::Matrix3Xf  V;
		V.resize(3, mesh.vertices_size());
		do {
			Point pt = pts[*vf];
			V.col(i++) = Eigen::Map<const Eigen::Vector3f>(pt.data());
		} while (++vf != vf_end);

		Point c_pt; Vector3f c_pt1; Vector3f c_pt2; Vector3f c_pt3;
		c_pt1 = V.col(0);
		c_pt2 = V.col(1);
		c_pt3 = V.col(2);
		Vector3f tmp = (c_pt1 + c_pt2 + c_pt3) / 3;
		c_pt.x = tmp(0), c_pt.y = tmp(1), c_pt.z = tmp(2);
		centroid[(f_it).idx()] = c_pt;

	}
}

void MeshDenose::getFaceNormal(const Surface_mesh &mesh, vector<Normal> &normals) const
{
	normals.resize(mesh.n_faces());
	for (auto f_it :mesh.faces())
	{
		Normal n = mesh.compute_face_normal(f_it);
		normals[(f_it).idx()] = n;
	}
}

//获取一个面相邻面的信息
void MeshDenose::getFaceFaceNeighbor(const Surface_mesh &mesh, Surface_mesh::Face f, vector<Surface_mesh::Face> &face_neighbor) const
{
	face_neighbor.clear();
	f.idx();
	std::set<int> neighbor_face_index;
	neighbor_face_index.clear();
	Surface_mesh::Vertex_around_face_circulator vf_it, vf_end;
	vf_it = vf_end = mesh.vertices(f);
	do
	{
		Surface_mesh::Face_around_vertex_circulator fv_it(&mesh, *vf_end), fv_end(&mesh, *vf_end);
		do
		{
			if ((*fv_it) != f)
				neighbor_face_index.insert((*fv_it).idx());
		} while (++fv_it != fv_end);
	} while (vf_it != vf_end);

	for (std::set<int>::iterator iter = neighbor_face_index.begin(); iter != neighbor_face_index.end(); ++iter)
	{
		face_neighbor.push_back(Surface_mesh::Face(*iter));
	}
}
