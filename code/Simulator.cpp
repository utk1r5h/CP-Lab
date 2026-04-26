#include "Simulator.h"
#include <iostream>
#include <algorithm>
#include <map>
using namespace std;

Simulator::Simulator(Graph& g) : graph(g) {
    day = 0;
}

// infect full population of patient zero city
void Simulator::infectCity(string cityName) {
    City* city = graph.findCity(cityName);
    if (!city) { cout << "[!] City not found" << endl; return; }
    if (city->isInfected) { cout << "[!] Already infected" << endl; return; }

    city->infectedCount  = city->population; // full city infected
    city->isInfected     = true;
    city->isFullyInfected = true;

    cout << "Day " << day << ": " << cityName
         << " is patient zero! (" << city->population
         << " people infected)" << endl;

   infectionLog.push_back({{cityName, city->population}});
}


void Simulator::quarantineCity(string cityName) {
    City* city = graph.findCity(cityName);
    if (!city) { cout << "[!] City not found" << endl; return; }

    city->isQuarantined = true;
    graph.closeAllRoads(cityName);
    cout << "Day " << day << ": " << cityName << " quarantined!" << endl;
}
void Simulator::stepOneDay() {
    day++;
    cout << "\n--- Day " << day << " ---" << endl;

    map<string, int> newInfections;

    for (City* city : graph.cities) {
        if (!city->isInfected || city->isQuarantined) continue;

        double ratio = (double)city->infectedCount / city->population;

        for (Road& road : city->roads) {
            if (road.isClosed) continue;

            City* neighbor = road.destination;
            if (neighbor->isQuarantined) continue;
            // dont spread to fully infected cities
            if (neighbor->isFullyInfected) continue;

            int spreading = (int)(ratio * road.capacity);
            if (spreading <= 0) continue;

            newInfections[neighbor->name] += spreading;
        }
    }

    // CHANGED — stores pair<cityName, newPeopleCount>
    // only logs cities that actually received new infections
    vector<pair<string, int>> dailyLog;

    for (auto& entry : newInfections) {
        City* city = graph.findCity(entry.first);
        if (!city) continue;

        int before = city->infectedCount;

        city->infectedCount += entry.second;
        if (city->infectedCount > city->population)
            city->infectedCount = city->population;

        int actualNewInfections = city->infectedCount - before;
        if (actualNewInfections <= 0) continue;

        // update flags
        if (city->infectedCount > 0)                   city->isInfected = true;
        if (city->infectedCount >= city->population)   city->isFullyInfected = true;

        cout << "  " << city->name
             << " | +" << actualNewInfections
             << " infected | total: " << city->infectedCount
             << "/" << city->population
             << " (" << (int)city->infectionPercent() << "%)" << endl;

        // CHANGED — only log if there were actual new infections
        dailyLog.push_back({city->name, actualNewInfections});
    }

    if (newInfections.empty()) {
        cout << "  No new infections today." << endl;
    }

    infectionLog.push_back(dailyLog);
}


void Simulator::printStatus() {
    cout << "\n=== Status on Day " << day << " ===" << endl;
    for (City* city : graph.cities) {
        cout << city->name
             << " | " << city->infectedCount << "/" << city->population
             << " (" << (int)city->infectionPercent() << "%)"
             << " | " << (city->isFullyInfected ? "FULLY INFECTED" :
                          city->isInfected      ? "PARTIALLY INFECTED" : "HEALTHY")
             << (city->isQuarantined ? " | QUARANTINED" : "")
             << endl;
    }
}



vector<City*> Simulator::getInfectedCities() {
    vector<City*> result;
    for (City* city : graph.cities)
        if (city->isInfected) result.push_back(city);
    return result;
}

vector<City*> Simulator::getHealthyCities() {
    vector<City*> result;
    for (City* city : graph.cities)
        if (!city->isInfected) result.push_back(city);
    return result;
}