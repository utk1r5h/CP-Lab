#pragma once
#include <string>
#include <vector>
#include "Road.h"
using namespace std;

class City {
public:
    string name;
    int population;
    bool isInfected;
    bool isQuarantined;
    vector<Road> roads;

    City(string name, int population);
};