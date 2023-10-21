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

Vector PolyMesh::getTriangleNormal(TriangleIndex t){
    Vector normal;
    Vector a = (vertex[t[1]] - vertex[t[0]]);
    Vector b = (vertex[t[2]] - vertex[t[0]]);
    a.cross(b, normal);
    normal.normalise();
    return normal;
}

void PolyMesh::addTriangle(TriangleIndex t, int P[], int normals[]){
    triangle.push_back({ P[t[0]], P[t[1]], P[t[2]] });

    // Vector normal;
    // for (int i : t){
    //     if (normals[i] == -1){
    //         normal = getTriangleNormal(t);
    //         break;
    //     }
    //     normal += vertexNormals[normals[i]];
    // }
    Vector normal = getTriangleNormal(triangle[triangle_count]);
    triangleNormals.push_back(normal);

    triangle_count++;
}

void PolyMesh::triangulatePolygon(int P[], int normals[], int n){
    if (n == 3){
        // Already a triangle
        addTriangle({0, 1, 2}, P, normals);
    }
    else if (n == 4){
        // Split quadralateral into two triangles
        addTriangle({0, 1, 2}, P, normals);
        addTriangle({0, 2, 3}, P, normals);
    }
    else{
        // Not currently implemented for n-polygons, n >= 5 
        return;
    }
}

PolyMesh::PolyMesh(char* file, bool smooth)
{
    this->smooth = smooth;

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

        if (lineHeader == "v"){
            // Verticies
            float x, y, z;
            iss >> x >> y >> z;
            vertex.push_back(Vertex(x, y, z));
            vertex_count++;
        }
        else if (lineHeader == "vt"){
            // Texture coordinates
            float x, y, z;
            iss >> x >> y >> z;
            textureCoords.push_back(Vertex(x, y, z));
            textureCoordsCount++;

        }
        else if (lineHeader == "vn"){
            // Vertex normals
            float x, y, z;
            iss >> x >> y >> z;
            vertexNormals.push_back(Vector(x, y, z));
            vertexNormalsCount++;

        }
        else if (lineHeader == "f"){
            // Faces
            int fvCount = 0;
            int faceVerticies[4];
            int faceVertexNormals[4];

            string faceVertex;
            // Get all vertex indicies
            while (iss >> faceVertex){
                istringstream vs(faceVertex);

                string vertexIndex;
                getline(vs, vertexIndex, '/');
                if (vertexIndex != ""){
                    // OBJ starts indexing at one, so correct
                    faceVerticies[fvCount] = stoi(vertexIndex) - 1;
                }

                string textureIndex;
                getline(vs, textureIndex, '/');
                // Doing nothing with texture for now
                
                string normalIndex;
                getline(vs, normalIndex, '/');
                if (normalIndex != ""){
                    faceVertexNormals[fvCount] = stoi(normalIndex) -1;
                }

                fvCount++;
            }

            triangulatePolygon(faceVerticies, faceVertexNormals, fvCount);
        }
    }
    modelFile.close();
}

bool PolyMesh::intersectsTriangle(Vertex p, int t, Vector normal){
    TriangleIndex tri = triangle[t];
    
    Vector v0 = vertex[tri[1]] - vertex[tri[0]];
    Vector v1 = vertex[tri[2]] - vertex[tri[1]];
    Vector v2 = vertex[tri[0]] - vertex[tri[2]];

    Vector a, b, c;

    (p - vertex[tri[0]]).cross(v0, a);
    if (normal.dot(a) > 0){
        return false;
    }

    (p - vertex[tri[1]]).cross(v1, b);
    if (normal.dot(b) > 0){
        return false;
    }

    (p - vertex[tri[2]]).cross(v2, c);
    if (normal.dot(c) > 0){
        return false;
    }

    return true;
}

Hit* PolyMesh::intersection(Ray ray)
{
    float best_t = 100000000.0f;
    Hit* best_hit = 0;
    for (int i = 0; i < triangle_count; i++){
        Vector normal = triangleNormals[i];

        float V = normal.dot(ray.direction);    
        
        if (V == 0){
            // Parallel, will not intersect
            continue;
        }
        
        float d = -normal.dot(vertex[(triangle[i])[0]]);
        float U = normal.dot(ray.position) + d;
        float t = -U / V;

        if (t < 0){
            // Intersection behind the ray
            continue;
        }
        
        if (t > best_t){
            continue;
        }

        Vertex P = ray.position + t * ray.direction;

        if (intersectsTriangle(P, i, normal)){
            if (best_hit ){
                delete best_hit;
            }

            Hit* hit = new Hit();
            hit->entering = true;
            hit->position = P;
            hit->t = t;
            hit->normal = normal;
            if (hit->normal.dot(ray.direction) > 0.0)
            {
                hit->normal.negate();
            }
            hit->what = this;
            hit->next = 0;
            
            best_t = t;
            best_hit = hit;
        }
    }

    if (best_hit){
        return best_hit;
    }

    return 0;
}

void PolyMesh::apply_transform(Transform& trans)
{
    // Transform each vertex
    for (int i = 0; i < vertex_count; i++){
        trans.apply(vertex[i]);
    }

    // Update normals
    for (int i = 0; i < triangle_count; i++){
        Vector normal = getTriangleNormal(triangle[i]);
        triangleNormals[i] = normal;
    }
}