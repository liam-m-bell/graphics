
#pragma once

#include "../Core/vector.h"
#include "../Core/colour.h"

enum photonType { normal, shadow, caustic };

struct Photon
{
  	Vector position;
	Vector direction;
	Colour energy;
	enum photonType type = normal;

  	Photon() {}
  	Photon(Vector p_position, Vector p_direction, Colour p_energy, enum photonType p_type){
		position = p_position;
		direction = p_direction;
		energy = p_energy;
		type = p_type;
	}

	float operator [] (int i) const {
		if (i == 0) { return position.x; }
		if (i == 1) { return position.y; }
		if (i == 2) { return position.z; }
	}

	bool operator == (const Photon& p) const {
		return position.x == p.position.x && position.y == p.position.y && position.z == p.position.z;
	}
};