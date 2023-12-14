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

void build_scene(Scene& scene){
	// Scene using models from https://www.kenney.nl/assets/holiday-kit
	// Deer model from https://free3d.com/3d-model/low-poly-deer-87801.html

	PolyMesh *snow = new PolyMesh((char *)"snowy.obj", false, true, &scene);
	Transform *transform = new Transform(
		10.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 10.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 10.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
	snow->apply_transform(*transform);
	scene.add_object(snow);
	
	Light *light = new PointLight(Vertex(-20.0f, 60.0f, 20.0f), Colour(0.2, 0.2, 0.2));
	scene.add_light(light);
	
	Material *blue = new GlobalMaterial(&scene, Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 1.0f, 
		(new Phong(Colour(0.0, 0.0, 0.1), Colour(0.0, 0.0, 0.0), Colour(0.0, 0.0, 0.0), 0)));

	Plane *sky = new Plane(-1, 0, 0, 20);
	sky->set_material(blue);
	scene.add_object(sky);
}

// This is the entry point function to the program.
int main(int argc, char *argv[])
{
	srand (time(NULL));

	int width = 2048;
	int height = 2048;
	// Create a framebuffer
	FrameBuffer* fb = new FrameBuffer(width, height);

	// Declare a camera
	Vertex position(-5.3f, 5.0f, 4.1f);
	Vector lookat(1.0f, 0.0f, -0.6f);
	Vector up(0.0f, 1.0f, 0.0f);

	Camera* camera = new FullCamera(0.4f, position, lookat, up);
	
	// Create a scene
	Scene scene;
	
	// Setup the scene
	build_scene(scene);
	
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
