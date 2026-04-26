#include "City.h"

City::City(string name, int population) {
    this->name = name;
    this->population = population;
    this->infectedCount=0;
    this->isInfected = false;
    this->isFullyInfected= false;
    this->isQuarantined = false;
}

double City::infectionPercent() const{
    if (population==0) return 0;
    return double(infectedCount)/population * 100;
}