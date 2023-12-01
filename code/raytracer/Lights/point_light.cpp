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

#include "point_light.h"

PointLight::PointLight()
{
	Light();
}

PointLight::PointLight(Vertex pos, Colour col)
{
	Light();

	position = pos;
	intensity = col;
}

bool PointLight::get_direction(Vertex &surface, Vector &dir)
{
	dir = (surface - position);
	dir.normalise();

	return true;
}

void PointLight::get_intensity(Vertex &surface, Colour &level)
{
	level = intensity;
}


// void PointLight::getPhotons(int n){
// 	n = 0;
// 	for (int i = 0; i < n; i++){
// 		Vector direction = Vector(2, 2, 2);
// 		do {
// 			direction.x = (2.0f * (float)(rand()) / (float)(RAND_MAX)) - 1.0f;
// 			direction.y = (2.0f * (float)(rand()) / (float)(RAND_MAX)) - 1.0f;
// 			direction.z = (2.0f * (float)(rand()) / (float)(RAND_MAX)) - 1.0f;
// 		} while (direction.len_sqr() > 1.0f);

// 		Ray photon;
// 		photon.position = position;
// 		photon.direciton = direction;

// 		n++;
// 	}

	
// }

