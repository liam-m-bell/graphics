
#pragma once

#include "../Core/environment.h"
#include "photon.h"
#include "kd/src/tree.h"

class PhotonMapper {
public:

    Environment environment;

    // typedef KD::Core<3, Photon, float> CORE;
    // KD::Tree<CORE> kd;

	PhotonMapper();
    PhotonMapper(Environment& env);

    void buildMap(int n);
    vector<Photon> query(Vector point, float radius);
};