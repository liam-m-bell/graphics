

#include <vector>

#include "photon_mapper.h"


PhotonMapper::PhotonMapper(Environment& env){
    environment = env;
}


void PhotonMapper::buildMap(int n){
    typedef KD::Core<3, Photon> CORE;

    Photon min(Vector(0,0,0), Vector(1,1,1), Colour(0,0,0), normal);
    Photon max(Vector(100,100,100), Vector(2,2,2), Colour(1,1,1), normal);
    KD::Tree<CORE> kdtree(min, max);
}

vector<Photon> PhotonMapper::query(Vector point, float radius){

}

