#include "Simulator.h"
#include <iostream>
using namespace std;

Simulator::Simulator(Graph& g) : graph(g) {
    day = 0;
}

void Simulator::infectCity(string cityName) {
    City* city = graph.findCity(cityName);
    if (city) {
        city->isInfected = true;
        cout << "Day " << day << ": " << cityName << " is patient zero!" << endl;
    }
}

void Simulator::quarantineCity(string cityName) {
    City* city = graph.findCity(cityName);
    if (city) {
        city->isQuarantined = true;
        cout << "Day " << day << ": " << cityName << " is quarantined!" << endl;
    }
}

void Simulator::stepOneDay() {
    day++;
    cout << "\n--- Day " << day << " ---" << endl;

    // collect newly infected this day
    // we dont directly modify while iterating
    vector<City*> newlyInfected;

    for (City* city : graph.cities) {
        if (city->isInfected && !city->isQuarantined) {
            for (Road& road : city->roads) {
                City* neighbor = road.destination;
                if (!neighbor->isInfected && !neighbor->isQuarantined) {
                    newlyInfected.push_back(neighbor);
                }
            }
        }
    }

    for (City* city : newlyInfected) {
        city->isInfected = true;
        cout << "  " << city->name << " got infected!" << endl;
    }

    if (newlyInfected.empty()) {
        cout << "  No new infections today." << endl;
    }
}

void Simulator::printStatus() {
    cout << "\n=== Status on Day " << day << " ===" << endl;
    for (City* city : graph.cities) {
        cout << city->name
             << " | Infected: " << (city->isInfected ? "YES" : "NO")
             << " | Quarantined: " << (city->isQuarantined ? "YES" : "NO")
             << endl;
    }
}

vector<City*> Simulator::getInfectedCities() {
    vector<City*> infected;
    for (City* city : graph.cities) {
        if (city->isInfected) infected.push_back(city);
    }
    return infected;
}

vector<City*> Simulator::getHealthyCities() {
    vector<City*> healthy;
    for (City* city : graph.cities) {
        if (!city->isInfected) healthy.push_back(city);
    }
    return healthy;
}