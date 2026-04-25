#include "Graph.h"
#include <iostream>
using namespace std;

Graph::Graph() {
    roadCounter = 0;
}

City* Graph::addCity(string name, int population) {
    City* city = new City(name, population);
    cities.push_back(city);
    return city;
}

void Graph::addRoad(City* a, City* b, string roadName, int distance, string type) {
    a->roads.push_back(Road(roadCounter, b, roadName, distance, type));
    b->roads.push_back(Road(roadCounter, a, roadName, distance, type));
    roadCounter++; // same ID for both directions of same road
}

City* Graph::findCity(string name) {
    for (City* city : cities) {
        if (city->name == name) return city;
    }
    return nullptr; // not found
}

void Graph::printGraph() {
    for (City* city : cities) {
        cout << "\n" << city->name << " (pop: " << city->population << ")";
        cout << " | Infected: " << (city->isInfected ? "YES" : "NO");
        cout << " | Quarantined: " << (city->isQuarantined ? "YES" : "NO");
        cout << "\n  Roads:";
        for (Road& r : city->roads) {
            cout << "\n    -> " << r.destination->name
                 << " | " << r.roadName
                 << " | " << r.distance << "km"
                 << " | " << r.type
                 << " | Road ID: " << r.id;
        }
        cout << endl;
    }
}