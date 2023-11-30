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
#include "Lights/point_light.h"

// classes that contain the materials applied to an object, all derived from Material
#include "Materials/phong_material.h"
#include "Materials/falsecolour_material.h"
#include "Materials/compound_material.h"
#include "Materials/global_material.h"

//classes that contain cameras, all derived from Camera
#include "Cameras/simple_camera.h"
#include "Cameras/full_camera.h"

using namespace std;

// you will find it useful during development/debugging to create multiple functions that fill out the scene.
void build_scene(Scene& scene)
{
	// Transform * teapotTransform = new Transform(
	// 	1.0f, 0.0f, 0.0f, 0.0f,
	// 	0.0f, 0.0f, 1.0f, 0.0f,
	// 	0.0f, 1.0f, 0.0f, 0.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f);

	// PolyMesh *pm = new PolyMesh((char *)"teapot.obj", false, false, &scene);
	// Transform * transform = new Transform(
	// 	0.12f, 0.0f, 0.0f, -3.0f,
	// 	0.0f, 0.12f, 0.0f, 0.2f,
	// 	0.0f, 0.0f, 0.12f, -6.0f,
	// 	0.0f, 0.0f, 0.0f, 1.0f);

	// pm->apply_transform(*teapotTransform);
	// pm->apply_transform(*transform);
	// pm->set_material(new Phong(Colour(0.01, 0.01, 0.01), Colour(0.5, 0.5, 0.5), Colour(0.1, 0.1, 0.1), 50));
	//scene.add_object(pm);

	PolyMesh *pm2 = new PolyMesh((char *)"craft.obj", false, true, &scene);
	//pm2->set_material(new Phong(Colour(0.05, 0, 0), Colour(0.1, 0, 0), Colour(0.3, 0, 0), 10));
	Transform * transform2 = new Transform(
		0.0f, 0.0f, 2.0f, 1.0f,
		0.0f, 2.0f, 0.0f, 0.2f,
		2.0f, 0.0f, 0.0f, -4.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	//pm2->apply_transform(*teapotTransform);
	pm2->apply_transform(*transform2);
	scene.add_object(pm2);
	
	Sphere *sp1 = new Sphere(Vertex(0.0f,0.5f,1.5f),0.5f);
	sp1->set_material(new Phong(Colour(0.05, 0, 0), Colour(0.1, 0, 0), Colour(0.3, 0, 0), 10));
	scene.add_object(sp1);
	
	Sphere *sp2 = new Sphere(Vertex(2.0f,1.0f,10.0f),1.0f);
	sp2->set_material(new Phong(Colour(0, 0.1, 0), Colour(0, 0.3, 0), Colour(0, 0.1, 0), 10));
	scene.add_object(sp2);

	

	Plane *p0 = new Plane(0, 0, 1, -20);
	CompoundMaterial *p0mat = new CompoundMaterial(2);
	p0mat->include_material(new Phong(Colour(0, 0, 0.01), Colour(0, 0, 0.3), Colour(0, 0, 0.1), 10));
	//p0mat->include_material(new GlobalMaterial(&scene, Colour(1, 1, 1), Colour(0.0, 0.0, 0.0), 1.0f));
	p0->set_material(p0mat);
	//scene.add_object(p0);

	Sphere *sp3 = new Sphere(Vertex(0.6f,2.0f,4.0f),2.0f);
	CompoundMaterial *sp3mat = new CompoundMaterial(2);
	sp3mat->include_material(new Phong(Colour(0.01, 0, 0.), Colour(0.1, 0, 0), Colour(0.1, 0.1, 0.1), 10));
	//sp3mat->include_material(new GlobalMaterial(&scene, Colour(0.2, 0.2, 0.2), Colour(0.5, 0.5, 0.5), 1.0f));
	sp3mat->include_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.5, 0.5, 0.5), 1.1f));
	sp3->set_material(sp3mat);
	//scene.add_object(sp3);

	Plane *p1 = new Plane(0, -1, 0, 0);
	p1->set_material(new Phong(Colour(0, 0, 0.03), Colour(0.2, 0.1, 0.5), Colour(0.1, 0.1, 0.1), 10));
	scene.add_object(p1);

	Plane *p2 = new Plane(0, 1, 0, -10);
	p2->set_material(new Phong(Colour(0, 0, 0.03), Colour(0.2, 0.1, 0.5), Colour(0.1, 0.1, 0.1), 10));
	//scene.add_object(p2);

	Plane *p3 = new Plane(-1, 0, 0, 10);
	p3->set_material(new Phong(Colour(0, 0, 0.03), Colour(0.2, 0.1, 0.5), Colour(0.1, 0.1, 0.1), 10));
	//scene.add_object(p3);

	Plane *p4 = new Plane(1, 0, 0, -10);
	p4->set_material(new Phong(Colour(0, 0, 0.03), Colour(0.2, 0.1, 0.5), Colour(0.1, 0.1, 0.1), 10));
	//scene.add_object(p4);

	Plane *p5 = new Plane(0, 0, -1, 20);
	p5->set_material(new Phong(Colour(0, 0, 0.03), Colour(0.2, 0.1, 0.5), Colour(0.1, 0.1, 0.1), 10));
	//scene.add_object(p5);

	Light *l1 = new DirectionalLight(Vector(-1.0f, -0.5f, 1.0f), Colour(0.1, 0.1, 0.1));
	scene.add_light(l1);

	// Light *l2 = new DirectionalLight(Vector(1.0f, -1.0f, 1.0f), Colour(0.2, 0.2, 0.2));
	// scene.add_light(l2);

	//Light *l3 = new DirectionalLight(Vector(1.0f, -0.5f, -1.0f), Colour(0.1, 0.1, 0.1));
	// scene.add_light(l3);

	Light *l0 = new PointLight(Vertex(-2.0f, 2.0f, -20.0f), Colour(0.1, 0.1, 0.1));
	scene.add_light(l0);

}


// This is the entry point function to the program.
int main(int argc, char *argv[])
{
	srand (time(NULL));

	int width = 1024;
	int height = 1024;
	// Create a framebuffer
	FrameBuffer* fb = new FrameBuffer(width, height);

	// Declare a camera
	//Camera *camera = new SimpleCamera(0.5f);
	Vertex position(2.0f, 5.0f, -10.0f);
	Vector lookat(0.0f, -0.2f, 1.0f);
	Vector up(0.0f, 1.0f, 0.0f);
	Camera* camera = new FullCamera(0.75f, position, lookat, up);
	
	// Create a scene
	Scene scene;
	
	// Setup the scene
	build_scene(scene);

	// Photon Mapping
	scene.photonMapping();
	
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
