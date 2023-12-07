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

// Get the reflected direction of a perfect specular reflection
Vector GlobalMaterial::specularReflection(Vector incident, Vector normal){
	incident.normalise();
	normal.normalise();
	Vector reflection = incident - (2.0f * incident.dot(normal) * normal);
	reflection.normalise();
	return reflection;
}

Colour GlobalMaterial::reflection(Vector& view, Hit& hit, int recurse){
	Colour result = Colour(0, 0, 0);

	Ray reflection;
	reflection.direction = specularReflection(view, hit.normal);

	// Start a little bit off the surface to stop self intersection
	float reflectedRayStartOffset = 0.0005;
	reflection.position = hit.position + (reflectedRayStartOffset * reflection.direction);

	float depth = 0.0f;
	
	// Raytrace, decrementing recurse to implement hard limit of reflection recursion depth
	environment->raytrace(reflection, recurse - 1, result, depth);

	return result;
}

Colour GlobalMaterial::refraction(Vector& view, Hit& hit, int recurse){
	Vector normal = hit.normal;
	normal.normalise();

	view.normalise();
	Vector I = -view;

	// Light exiting material, so relative ior of medium-boundary inverted
	float relativeIor = ior;
	if (!hit.entering){
		float relativeIor = 1/ior;
	}
	
	float cosThetaI = normal.dot(I);
	float cosThetaT = sqrt(1 - (1/pow(relativeIor, 2)) * (1 - pow(cosThetaI, 2)));

	// Calculate Fresnel term
	float rPar = pow(((relativeIor * cosThetaI) - cosThetaT) / ((relativeIor * cosThetaI) + cosThetaT), 2);
	float rPer = pow((cosThetaI - (relativeIor * cosThetaT)) / (cosThetaI + (relativeIor * cosThetaT)), 2);

	Colour kR = Colour(1, 1, 1) * 0.5f * (rPar + rPer);
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
	environment->raytrace(refractedRay, recurse, result, depth);

	if (isnan(result.r)){
		result = Colour();
	}

	result = refractWeight * kT * result ;

	Colour reflectResult = reflection(view, hit, recurse);
	if (isnan(reflectResult.r)){
		reflectResult = Colour();
	}


	result = result + (reflectResult * kR);

	return result;
}

// reflection and recursion computation
Colour GlobalMaterial::compute_once(Ray& viewer, Hit& hit, int recurse)
{
	Colour result = phongMat->compute_once(viewer, hit,  recurse);
	
	if (recurse != 0){
		if (refractWeight.r > 0 || refractWeight.g > 0|| refractWeight.b > 0){
			result += refraction(viewer.direction, hit, recurse);
		}
		if (reflectWeight.r > 0|| reflectWeight.g > 0|| reflectWeight.b > 0){
			result += reflectWeight * reflection(viewer.direction, hit, recurse);
		}
	}

	return result;
}

Colour GlobalMaterial::compute_per_light(Vector& viewer, Hit& hit, Vector& ldir)
{
	Colour result = phongMat->compute_per_light(viewer, hit, ldir);

	return result;
}

bool GlobalMaterial::receivePhoton(Photon *photon, Hit &hit){
	// Russian roulette
	float reflectProbability = 0.8f;
	float transmitProbability = 0.0f;

	float randomValue = (float)(rand()) / (float)(RAND_MAX);
	
	if (randomValue < reflectProbability) {
		return reflectPhoton(photon, hit);
	} else if (randomValue < reflectProbability + transmitProbability) {
		transmitPhoton(photon, hit);
		return false;
	} else {
		absorbPhoton(photon);
		return true;
	}
}

bool GlobalMaterial::reflectPhoton(Photon *photon, Hit &hit){
	// Russian roulette
	Colour diffuseEnergies = photon->energy;
	diffuseEnergies.scale(phongMat->kDiffuse);
	float diffuseProbability = max(diffuseEnergies.r, max(diffuseEnergies.g, diffuseEnergies.b)) / max(photon->energy.r, max(photon->energy.g, photon->energy.b));

	Colour specularEnergies = photon->energy;
	specularEnergies.scale(phongMat->kSpecular);
	float specularProbability = max(specularEnergies.r, max(specularEnergies.g, specularEnergies.b)) / max(photon->energy.r, max(photon->energy.g, photon->energy.b));

	float randomValue = (float)(rand()) / (float)(RAND_MAX);
	
	if (randomValue < diffuseProbability) {
		photon->direction = diffuseReflection(hit.normal);
		return true;

	} else if (randomValue < diffuseProbability + specularProbability) {
		photon->direction = specularReflection(photon->direction, hit.normal);
		photon->type = caustic;
		return false;
	} else {
		absorbPhoton(photon);
		return true;
	}
}

Vector GlobalMaterial::diffuseReflection(Vector normal){
	//Perpendicular vector to to normal found using https://blog.selfshadow.com/2011/10/17/perp-vectors/
	Vector a = Vector(abs(normal.x), abs(normal.y), abs(normal.z));
    Vector perp;

    if (a.x <= a.y && a.x <= a.z)
        perp = Vector(0, -normal.z, normal.y);
    else if (a.y <= a.x && a.y <= a.z)
        perp = Vector(-normal.z, 0, normal.x);
    else
        perp = Vector(-normal.y, normal.x, 0);

	perp.normalise();

	Vector perp2;
	perp.cross(normal, perp2);

	perp2.normalise();
	

	// Use cosine weighted reflection direction (https://pbr-book.org/3ed-2018/Monte_Carlo_Integration/2D_Sampling_with_Multidimensional_Transformations#ConcentricSampleDisk)
	std::default_random_engine generator;
	generator.seed(rand());
 	std::uniform_real_distribution<float> distribution(0.0f,1.0f);

	float u = distribution(generator);
	float v = distribution(generator);

	float x, y;

    //Handle degeneracy at the origin
    if (u == 0 && v == 0)
        x = 0;
		y = 0;

    // Apply concentric mapping to point 
    float theta, r;
	if (abs(u) > abs(v)) {
		r = u;
		theta = M_PI_4 * (v / u);
	} else {
		r = v;
		theta = M_PI_2 - M_PI_4 * (u / v);
	}
	x = r * cos(theta);
	y = r * sin(theta);

	float z2 = 1.0f - pow(x, 2) - pow(y, 2);
	float z = sqrt(max(0.0f, z2));

	Vector reflection = x * perp + y * perp2 + z * normal;
	reflection.normalise();

	return reflection;
}

void GlobalMaterial::transmitPhoton(Photon *photon, Hit &hit){
	//TODO
}

void GlobalMaterial::absorbPhoton(Photon *photon){
	photon->absorbed = true;
}