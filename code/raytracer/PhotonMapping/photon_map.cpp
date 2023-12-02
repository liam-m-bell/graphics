
#include "photon_map.h"


void PhotonMap::buildMap(std::vector<Photon> photons){
    for (Photon photon : photons){
        kdtree.insert(photon);
    }
}


std::vector<Photon> PhotonMap::query(Vector point, int n, float radius){
    Photon p;
    p.position = point;
    std::vector<Photon> nearbyPhotons = kdtree.find(p, n, (double)radius);
    return nearbyPhotons;
}

