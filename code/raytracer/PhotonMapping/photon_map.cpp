
#include "photon_map.h"


void PhotonMap::buildMap(std::vector<Photon> photons){
    for (Photon photon : photons){
        kdtree.insert(photon);
    }
}


std::vector<Photon> PhotonMap::query(Vector point, float radius){
    std::vector<Photon> nearbyPhotons;
    return nearbyPhotons;
}

