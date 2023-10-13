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
        triangle[triangle_count][0] = P[0];
        triangle[triangle_count][1] = P[1];
        triangle[triangle_count][2] = P[2];
        triangle_count++;
    }
    else if (n == 4){
        // Split quadralateral into two triangles
        triangle[triangle_count][0] = P[0];
        triangle[triangle_count][1] = P[1];
        triangle[triangle_count][2] = P[2];
        triangle_count++;
        triangle[triangle_count][0] = P[0];
        triangle[triangle_count][1] = P[3];
        triangle[triangle_count][2] = P[2];
        triangle_count++;
    }

    else{
        // Not implemented
        cout << "5";
        return;
    }
}

PolyMesh::PolyMesh(char* file, bool smooth)
{
    ifstream modelFile;
    modelFile.open(file);
    
    vertex_count = 0;
    triangle_count = 0;

    string line;
    while (getline(modelFile, line)){
        istringstream iss(line);
        string lineHeader;
        iss >> lineHeader;

        // Verticies
        if (lineHeader == "v"){
            float x, y, z;
            iss >> x >> y >> z;
            vertex[vertex_count] = Vertex(x, y, z);
            vertex_count++;
        }
        // Faces
        else if (lineHeader == "f"){
            int faceVerticies[4];
            int fvCount = 0;
            string faceVertex;

            while (iss >> faceVertex){
                istringstream vs(faceVertex);
                string vertexIndex;
                getline(vs, vertexIndex, '/');

                cout << vertexIndex;
                cout << " ";

                faceVerticies[fvCount] = stoi(vertexIndex) - 1;
                fvCount++;
            }
            cout << "\n";
            triangulatePolygon(faceVerticies, fvCount);
            cout << "Triangles: ";
            cout << triangle_count;
            cout << "\n";
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
    for (int i = 0; i < vertex_count; i++){
        trans.apply(vertex[i]);
    }
}