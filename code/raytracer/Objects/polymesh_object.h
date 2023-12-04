/****************************************************************************
*
* krt - Ken's Raytracer - Coursework Edition. (C) Copyright 1993-2022.
*
* I've put a lot of time and effort into this code. For the last decade
* it's been used to introduce hundreds of students at multiple universities
* to raytracing. It forms the basis of your coursework but you are free
* to continue using/developing forever more. However, I ask that you don't
* share the code or your derivitive versions publicly. In order to continue
* to be used for coursework and in particular assessment it's important that
* versions containing solutions are not searchable on the web or easy to
* download.
*
* If you want to show off your programming ability, instead of releasing
* the code, consider generating an incredible image and explaining how you
* produced it.
*/

// The PolyMesh Object reads and intersects with triangle meshes

#pragma once

#include <vector>
#include <array>
#include <map>
#include <string>

#include "../Core/object.h"
#include "../Core/material.h"
#include "../Core/environment.h"

typedef array<int, 3> TriangleIndex;

class PolyMesh:public Object{
public:

	Hit *intersection(Ray ray);
	void apply_transform(Transform& trans);
	void triangulatePolygon(int* P, int* normals, int n, string material);
	Vector getTriangleNormal(TriangleIndex t);
	void addTriangle(TriangleIndex t, int* P, int* normals, string material);
	bool intersectsTriangle(Vertex ray, int t, Vector normal);
	void calculateVertexNormals();
	Vector getInterpolatedNormal(int t, Vertex P);
	void loadMaterial(string file, Environment* p_env);
	Material *getFaceMaterial(string material);

    PolyMesh(char *file, bool smooth, bool useMaterial, Environment* env);
	~PolyMesh(){}

	bool smooth;

	int vertex_count;
	vector<Vertex> vertex;

	int textureCoordsCount;
	vector<Vertex> textureCoords;

	int normalsCount;
	vector<Vector> normals;

	int triangle_count;
	vector<TriangleIndex> triangle;

	vector<Vector> triangleNormals;

	vector<int> vertexNormals; 
	vector<vector<int>> vertexTriangles;

	int materialCount;
	map<string, Material*> materials;
	vector<string> triangleMaterials;

};
