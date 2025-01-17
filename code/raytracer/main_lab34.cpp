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
#include "Objects/quadratic_object.h"

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

void build_box(Scene& scene){
	Material *white = new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.01, 0.01, 0.01), Colour(0.5, 0.5, 0.5), Colour(0.1, 0.1, 0.1), 10)));

	Material *red = new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.01, 0.0, 0.0), Colour(0.5, 0.0, 0.0), Colour(0.1, 0.1, 0.1), 10)));

	Material *green = new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.0, 0.01, 0.0), Colour(0.0, 0.5, 0.0), Colour(0.1, 0.1, 0.1), 10)));

	Material *mirror = new GlobalMaterial(&scene, Colour(1.0, 1.0, 1.0), Colour(0.0, 0.0, 0.0), 1.0f, 
	(new Phong(Colour(0.01, 0.01, 0.01), Colour(0.05, 0.05, 0.05), Colour(0.1, 0.1, 0.1), 10)));


	Plane *floor = new Plane(0, 1, 0, 0);
	floor->set_material(white);
	scene.add_object(floor);

	Plane *back = new Plane(0, 0, -1, 5);
	back->set_material(white);
	scene.add_object(back);

	Plane *left = new Plane(1, 0, 0, 5);
	left->set_material(red);
	scene.add_object(left);

	Plane *right = new Plane(-1, 0, 0, 5);
	right->set_material(green);
	scene.add_object(right);

	Plane *ceiling = new Plane(0, -1, 0, 10);
	ceiling->set_material(white);
	scene.add_object(ceiling);

	Sphere *sphere = new Sphere(Vertex(1.0f,0.75f, 1.75f),0.75f);
	sphere->set_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0, 0.03, 0.02), Colour(0.0, 0.3, 0.2), Colour(0.1, 0.1, 0.1), 10))));
	scene.add_object(sphere);	

	Sphere *reflectiveSphere = new Sphere(Vertex(-1.8, 2.0f, 2.5f),2.0f);
	reflectiveSphere->set_material( new GlobalMaterial(&scene, Colour(0.95, 0.95, 0.95), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.0, 0.0, 0.01), Colour(0.0, 0.0, 0.1), Colour(0.1, 0.1, 0.1), 10))));

	reflectiveSphere->set_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.01, 0.01, 0.02), Colour(0.1, 0.12, 0.3), Colour(0.1, 0.1, 0.1), 10))));
	scene.add_object(reflectiveSphere);

	Sphere *sphere2 = new Sphere(Vertex(2.3f,1.0f, 3.5f),1.0f);
	sphere2->set_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.01, 0.01, 0.02), Colour(0.3, 0.12, 0.2), Colour(0.1, 0.1, 0.1), 10))));
	scene.add_object(sphere2);

	Sphere *refractiveSphere = new Sphere(Vertex(3.0f,1.0f,-0.5f),1.0f);
	refractiveSphere->set_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.95, 0.95, 0.95), 1.15f, 
		(new Phong(Colour(0.01, 0.0, 0.01), Colour(0.05, 0.0, 0.05), Colour(0.05, 0.05, 0.05), 10))));
	//scene.add_object(refractiveSphere);


	Light *light = new PointLight(Vertex(0.0f, 9.9f, 0.0f), Colour(0.2, 0.2, 0.2));
	scene.add_light(light);

	
	PolyMesh *craft = new PolyMesh((char *)"craft.obj", false, true, &scene);
	Transform *craftTransform = new Transform(
		-0.7071068f, 0.0f, -0.7071068f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.7071068f, 0.0f, -0.7071068f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	Transform *scaleT = new Transform(
		1.6f, 0.0f, 0.0f, 1.8f,
		0.0f, 1.6f, 0.0f, 0.2f,
		0.0f, 0.0f, 1.6f, 0.4f,
		0.0f, 0.0f, 0.0f, 1.0f);

	craft->apply_transform(*craftTransform);
	craft->apply_transform(*scaleT);


		PolyMesh *cube = new PolyMesh((char *)"cube.obj", true, false, &scene);
	cube->set_material(new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.95, 0.95, 0.95), 1.05f, 
		(new Phong(Colour(0.01, 0.01, 0.01), Colour(0.1, 0.1, 0.1), Colour(0.1, 0.1, 0.1), 10))));
	Transform *objectTransform = new Transform(
		1.0f, 0.0f, 0.0f, 2.0f,
		0.0f, 1.0f, 0.0f, 1.001f,
		0.0f, 0.0f, 1.0f, 0.3f,
		0.0f, 0.0f, 0.0f, 1.0f);
	Transform * teapotTransform = new Transform(
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	Transform * scaleTransform = new Transform(
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.4f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.1f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);

	cube->apply_transform(*scaleTransform);
	//cube->apply_transform(*teapotTransform);
	cube->apply_transform(*objectTransform);
	//scene.add_object(cube);

	//scene.add_object(craft);
}


// This is the entry point function to the program.
int main(int argc, char *argv[])
{
	srand (time(NULL));

	int width = 512;
	int height = 512;
	// Create a framebuffer
	FrameBuffer* fb = new FrameBuffer(width, height);

	// Declare a camera
	Vertex position(0.0f, 5.0f, -5.0f);
	Vector lookat(0.0f, 0.0f, 1.0f);
	Vector up(0.0f, 1.0f, 0.0f);

	Camera* camera = new FullCamera(0.4f, position, lookat, up);
	
	// Create a scene
	Scene scene;
	
	// Setup the scene
	//build_scene(scene);
	build_box(scene);

	// Photon Mapping
	scene.photonTraceDepth = 10;
	scene.photonMapping(30000);
	
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
