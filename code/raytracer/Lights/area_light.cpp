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

#include "area_light.h"

AreaLight::PointLight()
{
	Light();
}

AreaLight::PointLight(Vertex pos, Vector dir, float size, Colour col)
{
	Light();

	position = pos;
	normal = dir;
	size = size;
	intensity = col;
}

bool AreaLight::get_direction(Vertex &surface, Vector &dir)
{
	dir = (surface - position);
	dir.normalise();

	return true;
}

void AreaLight::get_intensity(Vertex &surface, Colour &level)
{
	level = intensity;
}


std::vector<Photon> AreaLight::getPhotons(int n){
	std::vector<Photon> photons;
	
	std::default_random_engine generator;
	std::uniform_real_distribution<float> squareDistribution(-0.5f * size, 0.5f * size);
 	std::uniform_real_distribution<float> distribution(-1.0f,1.0f);

	



	float u = distribution(generator);
	float v = distribution(generator);

	float theta = acos(sqrt(u));
	float phi = 2 * M_PI * v;

	float x = sin(theta) * cos(phi);
	float y = sin(theta) * sin(phi);
	float z = cos(theta);

	Vector reflection = Vector(x, y, z);

	x = x - (reflection.dot(normal) * normal.x);
	y = y - (reflection.dot(normal) * normal.y);
	z = z - (reflection.dot(normal) * normal.z);

	reflection = Vector(x, y, z);
	reflection.normalise();

	return reflection;










	for (int i = 0; i < n; i++){
		Vector direction;
		do {
			direction.x = distribution(generator);
			direction.y = distribution(generator);
			direction.z = distribution(generator);
		} while (direction.len_sqr() > 1.0f || direction.z < 0.0f  || direction.x > 0.2f || direction.x < -0.5f || direction.y > 0.2f || direction.y < -0.5f );

		Photon photon;
		photon.position = position;
		direction.normalise();
		photon.direction = direction;
		photon.energy = intensity * (1/(float)n);

		photons.push_back(photon);
	}

	return photons;
}

