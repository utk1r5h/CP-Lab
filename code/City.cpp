#include "City.h"

City::City(string name, int population) {
    this->name = name;
    this->population = population;
    this->isInfected = false;
    this->isQuarantined = false;
}