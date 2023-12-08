#pragma once

#include "../Core/material.h"

class MeshMaterial : public Material {
public:
	MeshMaterial(){

	}

	Colour compute_once(Ray& viewer, Hit& hit, int recurse);
	Colour compute_per_light(Vector& viewer, Hit& hit, Vector& ldir);
};
