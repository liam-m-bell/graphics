
#pragma once

#include <vector>

#include "photon.h"
#include "kd/src/tree.h"

class PhotonMap {
public:
    // typedef KD::Core<3, Photon, float> CORE;
    // KD::Tree<CORE> kd;

	PhotonMap();

    void buildMap(int n);
    std::vector<Photon> query(Vector point, float radius);
};