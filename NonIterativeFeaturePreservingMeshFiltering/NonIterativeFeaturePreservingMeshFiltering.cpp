// NonIterativeFeaturePreservingMeshFiltering.cpp: 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include "NonIterativeFeaturePreservingMeshFiltering.h"


void NonIterativeFeaturePreservingMeshFiltering::denoise(char** argv)
{
	//获取数据
	Surface_mesh mesh;
	mesh.read(argv[1]);

	if (mesh.n_vertices() == 0)
		return;

	std::vector<double> face_area;
	getFaceArea(mesh, face_area);

	std::vector<Point> face_centroid;
	getFaceCentroid(mesh, face_centroid);

	double sigma_f = 1, sigma_g = 1;

	std::vector<Normal> mollified_normals;
	mollifiedNormals(mesh, face_centroid, face_area, sigma_f, mollified_normals);

	std::vector<Point> new_point(mesh.n_vertices());
	for (auto v_it : mesh.vertices())
		new_point[(v_it).idx()] = mesh.position(v_it);

	for (auto v_it : mesh.vertices())
	{
		Point pt = mesh.position(v_it);

		std::vector<Surface_mesh::Face> vertex_face_neighbor;
		getFaceVertexNeighbor(mesh, face_centroid, v_it, sigma_f, vertex_face_neighbor);

		Point temp_point(0.0, 0.0, 0.0);
		double weight_sum = 0.0;

		for (int i = 0; i < (int)vertex_face_neighbor.size(); i++)
		{
			Point centroid = face_centroid[vertex_face_neighbor[i].idx()];
			Normal normal = mollified_normals[vertex_face_neighbor[i].idx()];
			Point projection_point = projectPoint(pt, centroid, normal);

			double distance_spatial = norm(centroid - pt);
			double weight_spatial = std::exp(-0.5 * distance_spatial * distance_spatial / (sigma_f * sigma_f));

			double distance_influence = norm(projection_point - pt);
			double weight_influence = std::exp(-0.5 * distance_influence * distance_influence / (sigma_g * sigma_g));

			double area = face_area[vertex_face_neighbor[i].idx()];

			temp_point += projection_point * area * weight_spatial * weight_influence;
			weight_sum += area * weight_spatial * weight_influence;
		}
		temp_point /= weight_sum;

		new_point.at(v_it.idx()) = temp_point;
	}
	
	Surface_mesh::Vertex_property<Point> pts = mesh.get_vertex_property<Point>("v:point");
	for (auto v_it : mesh.vertices())
	{
		pts[v_it] = new_point[(v_it).idx()];
	}
	/*for (auto v_it : mesh.vertices())
		mesh.position(v_it) = new_point[(v_it).idx()];*/

	mesh.write(argv[2]);
}

//获取顶点相邻的面信息
void NonIterativeFeaturePreservingMeshFiltering::getFaceVertexNeighbor(const Surface_mesh & mesh, vector<Point>& face_centroid,
	Surface_mesh::Vertex v, const double r, vector<Surface_mesh::Face> &vertex_face_neighbor) const
{
	double radius = 2.0*r;
	Point point = mesh.position(v);
	std::vector<bool> mark(mesh.n_faces(), false);
	vertex_face_neighbor.clear();
	std::queue<Surface_mesh::Face> queue_face;
	Surface_mesh::Face_around_vertex_circulator fv_it(&mesh, v), fv_end(&mesh, v);
	do
	{
		Point centroid = face_centroid[(*fv_it).idx()];
		double length = norm(point - centroid);
		if (length <= radius)
			queue_face.push(*fv_it);
		mark[(*fv_it).idx()] = true;
	} while (fv_it != fv_end);
	std::vector<Surface_mesh::Face> face_neighbor;
	while (!queue_face.empty())
	{
		Surface_mesh::Face f = queue_face.front();
		vertex_face_neighbor.push_back(f);
		queue_face.pop();
		getFaceFaceNeighbor(mesh, f, face_neighbor);
		for (int i = 0; i < (int)face_neighbor.size(); i++)
		{
			Surface_mesh::Face temp_fh = face_neighbor[i];
			if (mark[temp_fh.idx()] == false)
			{
				Point centroid = face_centroid[temp_fh.idx()];
				double length = norm(point - centroid);
				if (length <= radius)
					queue_face.push(temp_fh);
				mark[temp_fh.idx()] = true;
			}
		}
	}
}

void NonIterativeFeaturePreservingMeshFiltering::mollifiedNormals(const Surface_mesh & mesh, vector<Point>& face_centroid,
	vector<double>& face_area, double sigma_f, vector<Normal>& mollified_normals) const
{
	Surface_mesh temp_mesh = mesh;
	double sigma_m = sigma_f / 2.0;	//简化的方式平滑空间影响权sigmaf/2
	for (auto v_it : mesh.vertices())
	{
		Point pt = mesh.position(v_it);
		std::vector<Surface_mesh::Face> vertex_face_neighbor;
		getFaceVertexNeighbor(mesh, face_centroid, v_it, sigma_f, vertex_face_neighbor);

		Point new_point(0.0, 0.0, 0.0);
		double weight_sum = 0.0;
		for (int i = 0; i < (int)vertex_face_neighbor.size(); i++)
		{
			Point centroid = face_centroid[vertex_face_neighbor[i].idx()];

			double dis = norm(centroid - pt);
			double weight = std::exp(-0.5 * dis * dis / (sigma_m * sigma_m));

			double area = face_area[vertex_face_neighbor[i].idx()];
			new_point += centroid * area * weight;
			weight_sum += area * weight;
		}
		new_point /= weight_sum;
		Surface_mesh::Vertex_property<Point> pts = mesh.get_vertex_property<Point>("v:point");
		pts[v_it] = new_point;
	}
	mollified_normals.resize(mesh.n_faces());
	temp_mesh.update_face_normals();
	for (auto f_it : temp_mesh.faces())
		mollified_normals[(f_it).idx()] = temp_mesh.compute_face_normal(f_it);
}


Point NonIterativeFeaturePreservingMeshFiltering::projectPoint(Point pt, Point centroid, Normal n)
{
	//Eigen::Vector3f t_pt = Eigen::Map<Eigen::Vector3f>(pt.data());
	//Eigen::Vector3f t_centroid = Eigen::Map<Eigen::Vector3f>(centroid.data());
	//Eigen::Vector3f t_n = Eigen::Map<Eigen::Vector3f>(n.data());
	//Eigen::Vector3f t = t_pt - t_n * ((t_pt - t_centroid).dot(t_n));
	Point result;
	//result.x = t(0);
	//result.y = t(1);
	//result.z = t(2);
	float a = n.x, b = n.y, c = n.z;
	float x0 = centroid.x, y0 = centroid.y, z0 = centroid.z;
	float x = pt.x, y = pt.y, z = pt.z;
	float t;
	t = (a*x0 + b * y0 + c * z0 - (a*x + b * y + c * z)) / (a*a + b * b + c * c);
	result.x = x + a * t;
	result.y = y + b * t;
	result.z = z + c * t;
	return  result;
}
