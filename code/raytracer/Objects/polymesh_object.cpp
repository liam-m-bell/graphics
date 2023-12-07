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

#include "../Materials/phong_material.h"
#include "../Materials/global_material.h"
#include "../Materials/compound_material.h"
#include "../Materials/mesh_material.h"

using namespace std;

// Calculates the face normal for a triangle
Vector PolyMesh::getTriangleNormal(TriangleIndex t){
    Vector normal;
    Vector a = (vertex[t[1]] - vertex[t[0]]);
    Vector b = (vertex[t[2]] - vertex[t[0]]);
    a.cross(b, normal);
    normal.normalise();
    return normal;
}

// Calculates and stores the vertex normals for all verticies which don't already have a normal
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

// Adds a triangle to the mesh
void PolyMesh::addTriangle(TriangleIndex t, int P[], int N[], string material){
    Vector normal = getTriangleNormal({ P[t[0]], P[t[1]], P[t[2]] });
    if (isnan(normal.x)){
        return;
    }

    triangleNormals.push_back(normal);

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


    // Add material
    triangleMaterials.push_back(material);

    triangle_count++;
}

// Splits a polygon into triangles
void PolyMesh::triangulatePolygon(int P[], int N[], int n, string material){
    if (n == 3){
        // Already a triangle
        addTriangle({0, 1, 2}, P, N, material);
    }
    else if (n == 4){
        // Split quadralateral into two triangles
        addTriangle({0, 1, 2}, P, N, material);
        addTriangle({0, 2, 3}, P, N, material);
    }
    else{
        // Not currently implemented for n-polygons, n >= 5 
        return;
    }
}

PolyMesh::PolyMesh(char* file, bool smooth, bool useMaterial, Environment* env)
{
    this->smooth = smooth;

    vertex_count = 0;
    triangle_count = 0;
    normalsCount = 0;

    materialCount = 0;
    if (useMaterial){
        set_material(new MeshMaterial());
    }

    // Open OBJ
    ifstream modelFile;
    modelFile.open(file);

    // Material
    string faceMaterial;

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
                    // If no normal listed set to a placeholder index value
                    faceVertexNormals[fvCount] = -1;
                }

                fvCount++;
            }

            triangulatePolygon(faceVerticies, faceVertexNormals, fvCount, faceMaterial);
        }
        else if (lineHeader == "mtllib"){
            // Material Library
            string matFile;
            iss >> matFile;
            loadMaterial(matFile, env);
        }
        else if (lineHeader == "usemtl"){
            // Material 
            iss >> faceMaterial;
        }
    }
    modelFile.close();

    // Fill in any vertex normals which were not in the file
    calculateVertexNormals();
}

Vector PolyMesh::getInterpolatedNormal(int t, Vertex P){
    // Uses barycentric coordinates to interpolate the normal vectors for a point in a triangle
    // Method adapated from www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/barycentric-coordinates.html

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

    // Calculate interpolated normal using the u, v and w values
    Vector normal = u * normals[vertexNormals[tri[0]]] + v * normals[vertexNormals[tri[1]]] + w * normals[vertexNormals[tri[2]]];

    normal.normalise();
    return normal;
}

bool PolyMesh::intersectsTriangle(Vertex p, int t, Vector normal){
    TriangleIndex tri = triangle[t];
    
    Vector v0 = vertex[tri[1]] - vertex[tri[0]];
    Vector v1 = vertex[tri[2]] - vertex[tri[1]];
    Vector v2 = vertex[tri[0]] - vertex[tri[2]];

    // Check if the point is on the 'inside' of each of the edges of the triangle by checking cross product is in same direction as the normal
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
    // Find closest intersection
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
            // Not closer than best intersection
            continue;
        }

        // Intersection point
        Vertex P = ray.position + t * ray.direction;

        if (intersectsTriangle(P, i, normal)){
            // Intersects
            if (best_hit){
                delete best_hit;
            }

            Hit* hit = new Hit();
            hit->entering = true;
            hit->position = P;
            hit->t = t;

            if (smooth){
                // Use Phong Shading if 'smooth' set to true
                hit->normal = getInterpolatedNormal(i, P);
            }
            else{
                // Use face normals
                hit->normal = normal;
            }

            // Render both sides
            if (hit->normal.dot(ray.direction) > 0.0)
            {
                hit->normal.negate();
            }

            // Add material to Hit
            hit->material = triangleMaterials[i];

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

struct OBJMaterial{
    string name;
    Colour Ka;
    Colour Kd;
    Colour Ks;
    int Ns;
    Colour Tr;
    Colour Tf;
    float Ni = 1;
};

void PolyMesh::loadMaterial(string file, Environment* env){
    // Open Material file
    ifstream matFile;
    matFile.open(file);

    OBJMaterial faceMaterial;
    string line;
    // Read each line
    while (getline(matFile, line)){
        // Get line header
        istringstream iss(line);
        string lineHeader;
        iss >> lineHeader;

        if (lineHeader == "newmtl"){
            if (materialCount > 0){
                Phong *phongMat = new Phong(faceMaterial.Ka, faceMaterial.Kd, faceMaterial.Ks, faceMaterial.Ns);
                materials[faceMaterial.name] = new GlobalMaterial(env, faceMaterial.Tr, faceMaterial.Tf, faceMaterial.Ni, phongMat);
            }
            faceMaterial = OBJMaterial();
            string name;
            iss >> name;
            faceMaterial.name = name;
            materialCount++;
        }
        else if (lineHeader == "Ka"){
            float r, g, b;
            iss >> r >> g >> b;
            faceMaterial.Ka = Colour(r, g, b);
        }
        else if (lineHeader == "Kd"){
            float r, g, b;
            iss >> r >> g >> b;
            faceMaterial.Kd = Colour(r, g, b);
        }
        else if (lineHeader == "Ks"){
            float r, g, b;
            iss >> r >> g >> b;
            faceMaterial.Ks = Colour(r, g, b);
        }
        else if (lineHeader == "Ks"){
            int Ns;
            iss >> Ns;
            faceMaterial.Ns = Ns;
        }
        else if (lineHeader == "Tr"){
            float r, g, b;
            iss >> r >> g >> b;
            faceMaterial.Tr = Colour(r, g, b);
        }
        else if (lineHeader == "Tf"){
            float r, g, b;
            iss >> r >> g >> b;
            faceMaterial.Tf = Colour(r, g, b);
        }
        else if (lineHeader == "Ks"){
            float Ni;
            iss >> Ni;
            faceMaterial.Ni = Ni;
        }
    }

    if (materialCount > 0){
        Phong *phongMat = new Phong(faceMaterial.Ka, faceMaterial.Kd, faceMaterial.Ks, faceMaterial.Ns);
        materials[faceMaterial.name] = new GlobalMaterial(env, faceMaterial.Tr, faceMaterial.Tf, faceMaterial.Ni, phongMat);
    }

    matFile.close();
}

// Returns pointer to Material for a given name of a material
Material* PolyMesh::getFaceMaterial(string matName){
    Material *mat = materials[matName];
    return mat;
}