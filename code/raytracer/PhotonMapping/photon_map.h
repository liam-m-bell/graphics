
#pragma once

#include <vector>

#include "photon.h"
#include "kd/src/tree.h"


class PhotonMap {
public:
    KD::Tree<KD::Core<3, Photon>> kdtree;   

    Photon min;
    Photon max;
	PhotonMap(): min(Vector(0,0,0), Vector(1,1,1), Colour(0,0,0), normal), max(Vector(100,100,100), Vector(2,2,2), Colour(1,1,1), normal), kdtree(min, max) {}

    // void buildMap(int n);
    void buildMap(std::vector<Photon> photons);
    std::vector<Photon> query(Vector point, float radius);
};


