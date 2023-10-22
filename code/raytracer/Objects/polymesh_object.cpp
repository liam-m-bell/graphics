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

void PolyMesh::calculateVertexNormals(){
    for (int i = 0; i < vertex_count; i++){
        if (vertexNormals[i] == -1){
            Vector average = Vector(0, 0, 0);
            for (int face : vertexTriangles[i]){
                average = average + triangleNormals[face];
            }
            average.normalise();

            normals.push_back(average);
            vertexNormals[i] = normalsCount;
            normalsCount++;
        }
    }
}

void PolyMesh::addTriangle(TriangleIndex t, int P[], int N[]){
    triangle.push_back({ P[t[0]], P[t[1]], P[t[2]] });

    for (int i = 0; i < 3; i++){
        // Add face to vertex list
        vertexTriangles[P[t[i]]].push_back(triangle_count);

        // Add vertex normals
        int normalIndex = N[t[i]];
        if (normalIndex == -1){
            // No vertex normal was supplied, so will calculate normal from faces later once all faces are loaded
            vertexNormals[P[t[i]]] = -1; 
        }
        else{
            vertexNormals[P[t[i]]] = normalIndex;
        }
    }

    // Add face normal
    Vector normal = getTriangleNormal(triangle[triangle_count]);
    triangleNormals.push_back(normal);

    triangle_count++;
}

void PolyMesh::triangulatePolygon(int P[], int N[], int n){
    if (n == 3){
        // Already a triangle
        addTriangle({0, 1, 2}, P, N);
    }
    else if (n == 4){
        // Split quadralateral into two triangles
        addTriangle({0, 1, 2}, P, N);
        addTriangle({0, 2, 3}, P, N);
    }
    else{
        // Not currently implemented for n-polygons, n >= 5 
        return;
    }
}

PolyMesh::PolyMesh(char* file, bool smooth)
{
    this->smooth = smooth;

    vertex_count = 0;
    triangle_count = 0;
    normalsCount = 0;

    // Open OBJ
    ifstream modelFile;
    modelFile.open(file);

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

            // Add placeholder for vertex normal
            vertexNormals.push_back(-1);

            // Add placeholder for vertex triangles
            vertexTriangles.push_back({});

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
            normals.push_back(Vector(x, y, z));
            normalsCount++;

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
                else{
                    faceVertexNormals[fvCount] = -1;
                }

                fvCount++;
            }

            triangulatePolygon(faceVerticies, faceVertexNormals, fvCount);
        }
    }
    modelFile.close();

    // Fill in any vertex normals which were not in the file
    calculateVertexNormals();
}

Vector PolyMesh::getInterpolatedNormal(int t, Vertex P){
    TriangleIndex tri = triangle[t];
    Vertex v0 = vertex[tri[0]];
    Vertex v1 = vertex[tri[1]];
    Vertex v2 = vertex[tri[2]];

    Vector crossProduct;
    Vector edge = v2 - v1;
    Vector vP = P - v1;
    edge.cross(vP, crossProduct);
    float u = crossProduct.length() / 2;

    edge = v0 - v2;
    vP = P - v2;
    edge.cross(vP, crossProduct);
    float v = crossProduct.length() / 2;

    edge = v1 - v0;
    vP = P - v0;
    edge.cross(vP, crossProduct);
    float w = crossProduct.length() / 2;

    // Vector m, n, r, s;

    // r = C - A;
    // s = C - P;
    // r.cross(s, m);
    // s = C - B;
    // r.cross(s, n);
    // float a = m.length() / n.length();

    // r = A - B;
    // s = A - P;
    // r.cross(s, m);
    // s = A - C;
    // r.cross(s, n);
    // float b = m.length() / n.length();

    // r = B - C;
    // s = B - P;
    // r.cross(s, m);
    // s = B - A;
    // r.cross(s, n);
    // float c = m.length() / n.length();

    Vector normal = u * normals[vertexNormals[tri[0]]] + v * normals[vertexNormals[tri[1]]] + w * normals[vertexNormals[tri[2]]];

    normal.normalise();
    return normal;
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

            if (smooth){
                hit->normal = getInterpolatedNormal(i, P);
            }
            else{
                hit->normal = normal;
            }

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

    // Update face normals
    for (int i = 0; i < triangle_count; i++){
        Vector normal = getTriangleNormal(triangle[i]);
        triangleNormals[i] = normal;
    }

    // Update vertex normals
    for (int i = 0; i < normalsCount; i++){
        trans.apply(normals[i]);
    }
}