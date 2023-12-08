
#pragma once

#include <vector>

#include "photon.h"
#include "kd/src/tree.h"


class PhotonMap {
public:
    // Used KD Implementation from https://github.com/xavierholt/kd
    KD::Tree<KD::Core<3, Photon>> kdtree;

    std::vector<Photon> photons;  

    Photon min;
    Photon max;
	PhotonMap(): min(Photon(Vector(-20,-20,-20), Vector(0,0,0), Colour(0,0,0), direct)), max(Photon(Vector(20,20,20), Vector(0,0,0), Colour(0,0,0), direct)), kdtree(min, max) {}

    void addPhoton(Photon photon);
    void buildMap();
    std::vector<Photon> query(Vector point, int n);
};
