#include "stdafx.h"
#include "Mesh.h"

MyMesh::MyMesh()
{
}

MyMesh::MyMesh(const string file)
{
	BuildMeshFromFile(file);
}

void MyMesh::BuildMeshFromFile(const string filename)
{
	t_vertice.clear();
	t_face.clear();
	hf_edge.clear();
	fstream ifs(filename);
	string s;
	int p = 0;
	int num_v = 0; //记录顶点下标
	int num_f = 0; //记录面下标
	while (getline(ifs, s))
	{

		if (s.length() < 2)
		{
			continue;
		}
		//存储点信息
		if (s[0] == 'v')
		{
			Vertex temp_V;
			if (s[1] == 't' || s[1] == 'n')
			{
				continue;
			}
			else
			{
				istringstream in(s);
				string head;
				in >> head >> temp_V.x >> temp_V.y >> temp_V.z;
				num_v++;
				temp_V.idx = num_v;
			}
			t_vertice.push_back(temp_V);
		}
		//存储面信息
		else if (s[0] == 'f')
		{
			istringstream in(s);
			Face temp_F;
			//head暂存行首标识
			string head;
			in >> head;
			in >> temp_F.v[0];
			in >> temp_F.v[1];
			in >> temp_F.v[2];
			num_f++;
			temp_F.idx = num_f;
			t_face.push_back(temp_F);
		}
	}
	//测试是否存入数据
	/*for (vector<Face>::iterator i = t_face.begin(); i != t_face.end(); i++)
	{
	cout << "  " << i->v[0] << " " << i->v[1] << " " << i->v[2] << endl;
	}*/
	//建立半边
	for (f_iterator i = t_face.begin(); i != t_face.end(); i++)
	{
		//建立点的半边，某条边所在的面的信息
		for (int j = 0; j < 3; j++)
		{
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])] = new HalfEdge();
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->isBoundary = false;
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->startPoint = &(t_vertice[i->v[j] - 1]);
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->endPoint = &(t_vertice[i->v[(j + 1) % 3] - 1]);
			t_vertice[i->v[j] - 1].halfEdge = hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])];
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->face = &(*i);
		}
		//面的半边信息
		i->halfEdge = hf_edge[Pair(i->v[0], i->v[1])];
		//建立半边的下一条边
		for (int j = 0; j < 3; j++)
		{
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->nextHalfEdge = hf_edge[Pair(i->v[(j + 1) % 3], i->v[(j + 2) % 3])];
		}
		//建立半边的相反方向边
		for (int j = 0; j < 3; j++)
		{
			hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->nextHalfEdge = hf_edge[Pair(i->v[(j + 1) % 3], i->v[(j + 2) % 3])];
			if (hf_edge.find(Pair(i->v[(j + 1) % 3], i->v[j])) != hf_edge.end())
			{
				hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->oppositeHalfEdge = hf_edge[Pair(i->v[(j + 1) % 3], i->v[j])];
				hf_edge[Pair(i->v[(j + 1) % 3], i->v[j])]->oppositeHalfEdge = hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])];
			}
			else
				hf_edge[Pair(i->v[j], i->v[(j + 1) % 3])]->oppositeHalfEdge = NULL;
		}
	}
}

void MyMesh::SearchFace(int Vertex_ID)
{
	v_iterator i = t_vertice.begin();
	HalfEdge *start_edge = (i + Vertex_ID - 1)->halfEdge;
	HalfEdge *stop_edge = start_edge;
	do
	{
		cout << start_edge->face->idx << endl;
		start_edge = start_edge->oppositeHalfEdge->nextHalfEdge;
	} while (start_edge != stop_edge);
}

vector<int> MyMesh::SearchAdjV(size_t iD)
{

	v_iterator i = t_vertice.begin();
	HalfEdge *start_edge = (i + iD)->halfEdge;
	HalfEdge *stop_edge = start_edge;
	vector<int> adjV;
	do
	{
		adjV.push_back(start_edge->endPoint->idx);
		start_edge = start_edge->oppositeHalfEdge->nextHalfEdge;
	} while (start_edge != stop_edge);
	return adjV;
}

vector<Vertex> MyMesh::LaplacianSmoothing()
{
	vector<Vertex>  smooth_Vertex;
	Vertex v, t;
	int k = 0;
	vector<int> adj;
	for (v_iterator i = t_vertice.begin(); i != t_vertice.end(); i++, k++)
	{
		v.x = 0; v.y = 0; v.z = 0;
		adj = SearchAdjV(k);
		for (int j = 0; j < adj.size(); j++)
		{
			v.x = v.x + t_vertice[adj[j] - 1].x;
			v.y = v.y + t_vertice[adj[j] - 1].y;
			v.z = v.z + t_vertice[adj[j] - 1].z;
		}
		t.x = v.x / adj.size();
		t.y = v.y / adj.size();
		t.z = v.z / adj.size();
		smooth_Vertex.push_back(t);
	}
	return smooth_Vertex;
}

vector<Vertex> MyMesh::HCLaplacianSmoothing(vector<Vertex> q)
{
	double alpha = 0.2;
	double beta = 0.6;
	vector<Vertex> p = LaplacianSmoothing();
	vector<Vertex> b;
	Vertex t, bj;
	vector<int> adj;
	int k = 0;
	for (int i = 0; i < t_vertice.size(); i++)
	{
		t.x = p[i].x - (alpha*t_vertice[i].x + (1.0 - alpha)*q[i].x);
		t.y = p[i].y - (alpha*t_vertice[i].y + (1.0 - alpha)*q[i].y);
		t.z = p[i].z - (alpha*t_vertice[i].z + (1.0 - alpha)*q[i].z);
		b.push_back(t);
		t.x = 0; t.y = 0; t.z = 0;
	}
	for (int i = 0; i < t_vertice.size(); i++, k++)
	{
		adj = SearchAdjV(k);
		bj.x = 0;
		bj.y = 0;
		bj.z = 0;

		for (int j = 0; j < adj.size(); j++)
		{
			bj.x = bj.x + b[j].x;
			bj.y = bj.y + b[j].y;
			bj.z = bj.z + b[j].z;
		}
		p[i].x = p[i].x - (beta*b[i].x + (1.0 - beta) / adj.size()*bj.x);
		p[i].y = p[i].y - (beta*b[i].y + (1.0 - beta) / adj.size()*bj.y);
		p[i].z = p[i].z - (beta*b[i].z + (1.0 - beta) / adj.size()*bj.z);
	}
	return p;
}

vector<Vertex> MyMesh::HC_LaplacianSmoothing(int times)
{
	vector<Vertex> q(t_vertice);
	for (int i = 0; i < times; i++)
	{
		q = HCLaplacianSmoothing(q);
	}
	return q;
}

void MyMesh::RestructMesh(vector<Vertex> smooth_Vertex)
{
	fstream f("D:\\ITabc\\ITabc\\Paper_Reading\\MeshDenosing\\MeshDenosing\\MeshDenosing\\HCbp2.obj", ios::out);
	v_iterator i = smooth_Vertex.begin();
	f_iterator j = t_face.begin();
	for (i; i != smooth_Vertex.end(); i++)
	{
		f << 'v' << ' ';
		f << i->x << ' ' << i->y << ' ' << i->z << '\n';
	}
	f << '\n';
	for (j; j != t_face.end(); j++)
	{
		f << 'f' << ' ';
		f << j->v[0] << ' ' << j->v[1] << ' ' << j->v[2] << '\n';
	}
}
