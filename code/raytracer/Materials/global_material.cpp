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
#include <random>

GlobalMaterial::GlobalMaterial(Environment* p_env, Colour p_reflect_weight, Colour p_refract_weight, float p_ior, Phong *p_phongMat)
{
	environment = p_env;
	reflectWeight = p_reflect_weight;
	refractWeight = p_refract_weight;
	ior = p_ior;

	phongMat = p_phongMat;
}

Colour GlobalMaterial::reflection(Vector& view, Hit& hit, int recurse){
	Colour result = Colour(0, 0, 0);

	Ray reflection;
	reflection.direction = specularReflection(view, hit.normal);

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
	result = refractWeight * kT * result ;

	Colour reflectResult = reflection(view, hit, recurse);
	if (isnan(reflectResult.r)){
		reflectResult = Colour();
	}
	result = result + reflectResult * (kT * refractWeight + reflectWeight);

	return result;
}

// reflection and recursion computation
Colour GlobalMaterial::compute_once(Ray& viewer, Hit& hit, int recurse)
{
	Colour result = phongMat->compute_once(viewer, hit,  recurse);
	//Colour result = Colour(0, 0, 0);
	
	if (recurse != 0){
		result += refraction(viewer.direction, hit, recurse);
	}

	return result;
}

Colour GlobalMaterial::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	Colour result = phongMat->compute_per_light(viewer, hit, ldir);
	// Colour result;

	// result.r=0.0f;
	// result.g=0.0f;
	// result.b=0.0f;

	return result;
}

void GlobalMaterial::receivePhoton(Photon *photon, Hit &hit){
	// Russian roulette
	float reflectProbability = 0.5f;
	float transmitProbability = 0.0f;

	float randomValue = (float)(rand()) / (float)(RAND_MAX);
	
	if (randomValue < reflectProbability) {
		reflectPhoton(photon, hit);
	} else if (randomValue < reflectProbability + transmitProbability) {
		transmitPhoton(photon, hit);
	} else {
		absorbPhoton(photon, hit);
	}
}

void GlobalMaterial::reflectPhoton(Photon *photon, Hit &hit){
	// Russian roulette
	Colour diffuseEnergies = photon->energy;
	diffuseEnergies.scale(phongMat->kDiffuse);
	float diffuseProbability = max(diffuseEnergies.r, max(diffuseEnergies.g, diffuseEnergies.b)) / max(photon->energy.r, max(photon->energy.g, photon->energy.b));

	Colour specularEnergies = photon->energy;
	specularEnergies.scale(phongMat->kSpecular);
	float specularProbability = max(specularEnergies.r, max(specularEnergies.g, specularEnergies.b)) / max(photon->energy.r, max(photon->energy.g, photon->energy.b));

	float randomValue = (float)(rand()) / (float)(RAND_MAX);
	
	if (randomValue < diffuseProbability) {
		diffuseReflection(hit.normal);

	} else if (randomValue < diffuseProbability + specularProbability) {
		//specularReflection
	} else {
		absorbPhoton(photon, hit);
	}
}

Vector GlobalMaterial::diffuseReflection(Vector normal){
	// Use cosine weighted reflection direction (Jensen Realistic Image Synthesis using photon mapping)
	std::default_random_engine generator;
 	std::uniform_real_distribution<float> distribution(0.0f,1.0f);

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
}

Vector GlobalMaterial::specularReflection(Vector incident, Vector normal){
	incident.normalise();
	normal.normalise();
	Vector reflection = incident - (2.0f * incident.dot(normal) * normal);
	reflection.normalise();
	return reflection;
}

void GlobalMaterial::transmitPhoton(Photon *photon, Hit &hit){
	//TODO
}

void GlobalMaterial::absorbPhoton(Photon *photon, Hit &hit){
	photon->absorbed = true;
}