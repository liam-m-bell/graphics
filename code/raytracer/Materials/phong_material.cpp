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

 // A simple Phong based lighting model.

#include "phong_material.h"

#include <math.h>

Phong::Phong(Colour p_ambient, Colour p_diffuse, Colour p_specular, int p_power)
{
	kAmbient = p_ambient;
	kDiffuse = p_diffuse;
	kSpecular = p_specular;
	n = p_power;
}

// The compute_once() method supplies the ambient term.
Colour Phong::compute_once(Ray& viewer, Hit& hit, int recurse)
{
	Colour result = kAmbient;
	return result;
}

// The compute_per_light() method supplies the diffuse and specular terms.
Colour Phong::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	Colour result;
	Vector normal = hit.normal;
	float ndotl = normal.dot(-ldir);

	if (ndotl < 0.0f) // light is behind surface
	{
		return result;
	}
	
	if (ndotl > 0){
		Colour diffuse = kDiffuse * ndotl;
		result.add(diffuse);
	}
	
	Vector reflection = ldir - 2.0f * (ldir).dot(normal) * normal;
	if (reflection.dot(-viewer) > 0.0f)
	{
		Colour specular = kSpecular * pow(reflection.dot(-viewer), n);
		result.add(specular);
	}

	return result;
}

