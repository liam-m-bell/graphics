
#pragma once

#include <vector>

#include "photon.h"
#include "kd/src/tree.h"


class PhotonMap {
public:

    KD::Tree<KD::Core<3, Photon>> kdtree;

    std::vector<Photon> photons;  

    Photon min;
    Photon max;
	PhotonMap(): min(Photon(Vector(-50,-50,-50), Vector(0,0,0), Colour(0,0,0), normal)), max(Photon(Vector(50,50,50), Vector(0,0,0), Colour(0,0,0), normal)), kdtree(min, max) {}

    void addPhoton(Photon photon);
    void buildMap();
    std::vector<Photon> query(Vector point, float radius);
};


