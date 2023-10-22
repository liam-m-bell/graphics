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

/* This is the top level for the program you need to create for lab three and four.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

// these are Core raytracing classes
#include "Core/framebuffer.h"
#include "Core/scene.h"

// classes that contain our objects to be rendered, all derived from Object
#include "Objects/polymesh_object.h"
#include "Objects/sphere_object.h"
#include "Objects/plane_object.h"

// classes that contain our lights, all derived from Light
#include "Lights/directional_light.h"

// classes that contain the materials applied to an object, all derived from Material
#include "Materials/phong_material.h"
#include "Materials/falsecolour_material.h"

//classes that contain cameras, all derived from Camera
#include "Cameras/simple_camera.h"
#include "Cameras/full_camera.h"

using namespace std;

// you will find it useful during development/debugging to create multiple functions that fill out the scene.
void build_scene(Scene& scene)
{
	PolyMesh *pm = new PolyMesh((char *)"deer-low.obj", true);
	Transform * transform = new Transform(
		0.5f, 0.0f, 0.0f, -2.0f,
		0.0f, 0.5f, 0.0f, 0.5f,
		0.0f, 0.0f, 0.5f, -5.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	Transform * scaleTransform = new Transform(
		0.15f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.15f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.15f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	Transform * teapotTransform = new Transform(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	
	pm->apply_transform(*scaleTransform);
	//pm->apply_transform(*teapotTransform);
	pm->apply_transform(*transform);
	pm->set_material(new Phong(Colour(20, 20, 20), Colour(100, 100, 100), Colour(0, 0, 0), 10));
	//pm->set_material(new FalseColour());
	scene.add_object(pm);
	
	Sphere *sp1 = new Sphere(Vertex(3.0f,1.0f,2.0f),1.0f);
	sp1->set_material(new Phong(Colour(10, 0, 0), Colour(100, 0, 0), Colour(30, 0, 0), 10));
	scene.add_object(sp1);
	
	Sphere *sp2 = new Sphere(Vertex(1.0f,0.5f,-7.0f),0.5f);
	sp2->set_material(new Phong(Colour(0, 10, 0), Colour(0, 50, 0), Colour(0, 20, 0), 10));
	scene.add_object(sp2);

	Sphere *sp3 = new Sphere(Vertex(1.0f,1.5f,4.0f),1.5f);
	sp3->set_material(new Phong(Colour(0, 0, 10), Colour(0, 0, 80), Colour(0, 0, 10), 10));
	scene.add_object(sp3);

	Plane *p1 = new Plane(0, -1, 0, 0);
	p1->set_material(new Phong(Colour(0, 0, 50), Colour(30, 20, 80), Colour(0, 0, 10), 10));
	scene.add_object(p1);

	Light *l1 = new DirectionalLight(Vector(-0.5, 0, 0.1), Colour(15, 15, 15));
	scene.add_light(l1);

	Light *l2 = new DirectionalLight(Vector(0.1, -0.9, 0.9), Colour(50, 50, 50));
	scene.add_light(l2);
}


// This is the entry point function to the program.
int main(int argc, char *argv[])
{
	int width = 512;
	int height = 512;
	// Create a framebuffer
	FrameBuffer* fb = new FrameBuffer(width, height);
	
	// Create a scene
	Scene scene;
	
	// Setup the scene
	build_scene(scene);
	
	// Declare a camera
	//Camera *camera = new SimpleCamera(0.5f);
	Vertex position(0.0f, 4.0f, -18.0f);
	Vector lookat(0.0f, -0.2f, 1.0f);
	Vector up(0.0f, 1.0f, 0.0f);
	Camera* camera = new FullCamera(0.5f, position, lookat, up);
	
	// Camera generates rays for each pixel in the framebuffer and records colour + depth.
	camera->render(scene,*fb);
	
	// Output the framebuffer colour and depth as two images
	fb->writeRGBFile((char *)"test.ppm");
	fb->writeDepthFile((char *)"depth.ppm");
	
	cerr << "\nDone.\n" << flush;
	
	// a debug check to make sure we don't leak hits.
	cerr << "Hit Pool " << Hit::pool_size << " Allocated " << Hit::allocated << "\n" << flush;
	return 0;
}
