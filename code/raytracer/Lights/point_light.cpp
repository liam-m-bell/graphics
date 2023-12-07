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

#include <random>

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


float PointLight::getDistance(Vertex point){
	return (point - position).length();
}


std::vector<Photon> PointLight::getPhotons(int n){
	std::default_random_engine generator;
 	std::uniform_real_distribution<float> distribution(-1.0f,1.0f);

	std::vector<Photon> photons;

	for (int i = 0; i < n; i++){
		Vector direction;
		do {
			direction.x = distribution(generator);
			direction.y = distribution(generator);
			direction.z = distribution(generator);
		} while (direction.len_sqr() > 1.0f || direction.y >= 0); // Also reject directions which don't point downwards. Yes this is hard coded as this is scene specfic atm.

		Photon photon;
		photon.position = position;
		direction.normalise();
		photon.direction = direction;

		// Distribute the lights totale energy evenly for all the photons
		photon.energy =  intensity * (1500/(float)n);

		photons.push_back(photon);
	}

	return photons;
}

