#include <iostream>
#include "photon_map.h"

void PhotonMap::addPhoton(Photon photon){
    photons.push_back(photon);
}

void PhotonMap::buildMap(){
    std::cout << photons.size();
    
    // Add photons to the KD Tree
    for (int i = 0; i < photons.size(); i++){
        kdtree.insert(photons[i]);
        
        // Print progress
        if ((photons.size() > 20) && i%(photons.size() / 20) == 0){
			std::cout << "\nBuilding Photon Map:" <<(100 * i/photons.size()) << "%";
		}
    }
}

// Get the n nearest photons to a point
std::vector<Photon> PhotonMap::query(Vector point, int n){
    Photon p;
    p.position = point;
    std::vector<Photon> nearbyPhotons = kdtree.nearest(p, n);
    return nearbyPhotons;
}

