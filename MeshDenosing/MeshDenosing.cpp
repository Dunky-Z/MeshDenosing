#include "stdafx.h"
#include <stdlib.h>   
#include <iostream>
#include "Mesh.h"

using namespace std;

int main()
{
	cout << "LA：拉普拉斯平滑" << endl;
	cout << "HC:HC-Algorithm" << endl;
	cout << "请输入对应的字母： " << endl;
	string name;
	size_t times;
	cin >> name;
	MyMesh mesh("bumpy_torus.obj");
	vector<Vertex> smooth_Vertex;
	if (name == "LA")
	{
		smooth_Vertex = mesh.LaplacianSmoothing();
		mesh.RestructMesh(smooth_Vertex);
	}
	else if(name == "HC")
	{
		cout << "请输入迭代次数: " << endl;
		cin >> times;
		smooth_Vertex = mesh.HC_LaplacianSmoothing(times);
		mesh.RestructMesh(smooth_Vertex);
	}
	system("pause");
	return 0;
}

