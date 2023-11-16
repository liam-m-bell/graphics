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

#include "global_material.h"

#include <math.h>

GlobalMaterial::GlobalMaterial(Environment* p_env, Colour p_reflect_weight, Colour p_refract_weight, float p_ior)
{
	environment = p_env;
	reflectWeight = p_reflect_weight;
	refractWeight = p_refract_weight;
	ior = p_ior;
}

Colour GlobalMaterial::reflection(Vector& view, Hit& hit, int recurse){
	Colour result = Colour(0, 0, 0);

	Ray reflection;
	view.normalise();
	Vector normal = hit.normal;
	normal.normalise();

	reflection.direction = view - (2.0f * (view.dot(hit.normal)) * hit.normal);
	reflection.direction.normalise();
	// Start a little bit off the surface to stop self intersection
	float reflectedRayStartOffset = 0.0005;
	reflection.position = hit.position + (reflectedRayStartOffset * reflection.direction);

	float depth = 1;
	environment->raytrace(reflection, recurse - 1, result, depth);

	return result;
}

Colour GlobalMaterial::refraction(Vector& view, Hit& hit, int recurse){
	Vector normal = hit.normal;
	normal.normalise();

	view.normalise();
	Vector I = -view;

	float relativeIor = ior;
	if (!hit.entering){
		float relativeIor = 1/ior;
	}
	
	float cosThetaI = normal.dot(I);
	float cosThetaT = sqrt(1 - (1/pow(relativeIor, 2)) * (1 - pow(cosThetaI, 2)));

	float rPar = ((relativeIor * cosThetaI) - cosThetaT) / ((relativeIor * cosThetaI) + cosThetaT);
	float rPer = (cosThetaI - (relativeIor * cosThetaT)) / (cosThetaI + (relativeIor * cosThetaT));

	Colour kR = Colour(1, 1, 1) * 0.5f * (pow(rPar, 2) + pow(rPer, 2));
	Colour kT  = Colour(1, 1, 1) + (kR * Colour(-1, -1, -1));

	// Calculate refraction
	Colour result = Colour(0, 0, 0);
	Ray refractedRay;
	refractedRay.direction = (1/relativeIor) * view - (cosThetaT - ((1/relativeIor) * cosThetaI)) * normal;
	refractedRay.direction.normalise();

	// Start a little bit off the surface to stop self intersection
	float refractedRayStartOffset = 0.005;
	refractedRay.position = hit.position + (refractedRayStartOffset * refractedRay.direction);

	float depth = 1.0f;
	environment->raytrace(refractedRay, recurse - 1, result, depth);

	if (isnan(result.r)){
		result = Colour();
	}
	result.scale(kT);
	result.scale(refractWeight);

	Colour reflectResult = reflection(view, hit, recurse);
	if (isnan(reflectResult.r)){
		reflectResult = Colour();
	}
	reflectResult.scale(kR);
	reflectResult.scale(reflectWeight);

	result.add(reflectResult);

	return result;
}

// reflection and recursion computation
Colour GlobalMaterial::compute_once(Ray& viewer, Hit& hit, int recurse)
{
	Colour result = Colour(0, 0, 0);
	
	if (recurse != 0){
		result = refraction(viewer.direction, hit, recurse);
	}

	return result;
}

Colour GlobalMaterial::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	Colour result;

	result.r=0.0f;
	result.g=0.0f;
	result.b=0.0f;

	return result;
}

