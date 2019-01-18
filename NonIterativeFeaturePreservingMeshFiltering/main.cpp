
#include "stdafx.h"
#include "NonIterativeFeaturePreservingMeshFiltering.h"

int main(int argc, char **argv)
{
	NonIterativeFeaturePreservingMeshFiltering meshdenose;
	meshdenose.denoise(argv);
	//vector<Point> face_centroid;
	//meshdenose.getFaceCentroid(mesh, face_centroid);
	////cout << centroid.size() << endl;
	////meshdenose.denoise(&argv[1]);


	//std::vector<double> face_area;
	//meshdenose.getFaceArea(mesh, face_area);


	/*vector<Normal> normals;
	meshdenose.getFaceNormal(mesh, normals);
	cout << normals.size() << endl; 
*/
/*vector<Surface_mesh::Face> face_neighbor;
Surface_mesh::Face f(10);
meshdenose.getFaceFaceNeighbor(mesh, f, face_neighbor);*/

	//double sigma_f = 1;

	//vector<Normal> mollified_normals;
	//meshdenose.mollifiedNormals(mesh, face_centroid, face_area, sigma_f, mollified_normals);

	return 0; 
}
