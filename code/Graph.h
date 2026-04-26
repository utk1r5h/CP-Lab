#pragma once
#include <vector>
#include <string>
#include "City.h"
#include "Road.h"
#include "DSU.h"
using namespace std;

class Graph {
private:
    int roadCounter; // gives each road a unique ID

public:
    vector<City*> cities;

    Graph();
    City* addCity(string name, int population);

    void addRoad(City* a, City* b, string roadName, int distance, string type, int capacity);
    City* findCity(string name);
    void printGraph();

    void closeRoad(string cityA, string cityB, string roadName);

    void closeAllRoads(string cityName);
    DSU buildDSU();
    void printConnectivity();
};