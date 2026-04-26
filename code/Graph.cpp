


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

void Graph::addRoad(City* a, City* b, string roadName, int distance, string type, int capacity) {
    a->roads.push_back(Road(roadCounter, b, roadName, distance, type, capacity));
    b->roads.push_back(Road(roadCounter, a, roadName, distance, type, capacity));
    roadCounter++;
}

City* Graph::findCity(string name) {
    for (City* city : cities) {
        if (city->name == name) return city;
    }
    return nullptr;
}

void Graph::printGraph() {
    for (City* city : cities) {
        cout << "\n" << city->name
             << " | Pop: " << city->population
             << " | Infected: " << city->infectedCount
             << " (" << city->infectionPercent() << "%)"
             << " | Quarantined: " << (city->isQuarantined ? "YES" : "NO")
             << endl;
        cout << "  Roads:" << endl;
        for (Road& r : city->roads) {
            cout << "    -> " << r.destination->name
                 << " | " << r.roadName
                 << " | " << r.distance << "km"
                 << " | capacity: " << r.capacity << "/day"
                 << " | " << (r.isClosed ? "CLOSED" : "OPEN")
                 << endl;
        }
    }
}


void Graph::closeRoad(string cityAName, string cityBName, string roadName){
    City* cityA= findCity(cityAName);
    City* cityB = findCity(cityBName);

    if(!cityA || !cityB){
        cout << "City not found" << endl;
    }

    bool found = false;

    for(Road& r : cityA->roads){
        if(r.destination->name==cityBName && r.roadName ==roadName){
            r.isClosed=true;
            found= true;
        }
    }

    for(Road& r : cityB->roads){
        if(r.destination->name == cityAName && r.roadName==roadName){
            r.isClosed= true;
        }

    }

    if(found){
        cout<< "[CLOSED]" << roadName << " between "
        << cityAName << " and "<< cityBName << endl;
    }
    else{
        cout<< " Road not found! "<<endl;

    }
}

void Graph:: closeAllRoads(string cityName){
    City* city = findCity(cityName);
    if (!city){
        cout<< " [!] city not found"<< endl; 
        return;
    }

    for( Road& r: city->roads){
        r.isClosed= true;

        for(Road& rr : r.destination->roads){
            if (rr.destination->name == cityName){
                rr.isClosed= true;
            }

        }
    }

    cout<< "[CLOSED ALL] All roads of "<< cityName << " are closed"<<endl;
}