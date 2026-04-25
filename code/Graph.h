#pragma once
#include <vector>
#include <string>
#include "City.h"
#include "Road.h"
using namespace std;

class Graph {
private:
    int roadCounter; // gives each road a unique ID

public:
    vector<City*> cities;

    Graph();
    City* addCity(string name, int population);
    void addRoad(City* a, City* b, string roadName, int distance, string type);
    City* findCity(string name);
    void printGraph();
};