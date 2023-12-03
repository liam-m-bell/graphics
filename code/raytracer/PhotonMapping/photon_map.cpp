#include <iostream>
#include "photon_map.h"

void PhotonMap::addPhoton(Photon photon){
    photons.push_back(photon);
}

void PhotonMap::buildMap(){
    std::cout << photons.size();
    
    for (int i = 0; i < photons.size(); i++){
        kdtree.insert(photons[i]);
        
        if ((photons.size() > 20) && i%(photons.size() / 20) == 0){
			std::cout << "\nBuilding Photon Map:" <<(100 * i/photons.size()) << "%";
		}
    }
}


std::vector<Photon> PhotonMap::query(Vector point, int n){
    Photon p;
    p.position = point;
    std::vector<Photon> nearbyPhotons = kdtree.nearest(p, n);
    return nearbyPhotons;
}

