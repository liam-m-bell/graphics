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

#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "polymesh_object.h"

using namespace std;


void PolyMesh::triangulatePolygon(int P[], int n){
    if (n == 3){
        // Already a triangle
        TriangleIndex t = { P[0], P[1], P[2] };
        triangle.push_back(t);
        triangle_count++;
    }
    else if (n == 4){
        // Split quadralateral into two triangles
        TriangleIndex a = { P[0], P[1], P[2] };
        triangle.push_back(a);

        TriangleIndex b = { P[0], P[3], P[2] };
        triangle.push_back(b);

        triangle_count += 2;
    }

    else{
        // Not currently implemented for n-polygons, n >= 5 
        return;
    }
}

PolyMesh::PolyMesh(char* file, bool smooth)
{
    // Open OBJ
    ifstream modelFile;
    modelFile.open(file);
    
    vertex_count = 0;
    triangle_count = 0;

    string line;
    // Read each line
    while (getline(modelFile, line)){
        // Get line header
        istringstream iss(line);
        string lineHeader;
        iss >> lineHeader;

        // Verticies
        if (lineHeader == "v"){
            float x, y, z;
            iss >> x >> y >> z;
            vertex.push_back(Vertex(x, y, z));
            vertex_count++;
        }
        // Faces
        else if (lineHeader == "f"){
            int faceVerticies[4];
            int fvCount = 0;
            string faceVertex;

            // Get all vertex indicies
            while (iss >> faceVertex){
                istringstream vs(faceVertex);
                string vertexIndex;
                getline(vs, vertexIndex, '/');

                // OBK starts indexing at one, so correct
                faceVerticies[fvCount] = stoi(vertexIndex) - 1;
                fvCount++;
            }

            triangulatePolygon(faceVerticies, fvCount);
        }
        else{

        }
    }
    modelFile.close();
}



Hit* PolyMesh::intersection(Ray ray)
{
    Hit* hits = 0;

    return hits;
}

void PolyMesh::apply_transform(Transform& trans)
{
    // Transform each vertex
    for (int i = 0; i < vertex_count; i++){
        trans.apply(vertex[i]);
    }
}