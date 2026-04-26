#include "Simulator.h"
#include <iostream>
#include <algorithm>
#include <map>
using namespace std;

Simulator::Simulator(Graph& g) : graph(g) {
    day = 0;
}

void Simulator::infectCity(string cityName) {
    City* city = graph.findCity(cityName);
    if (!city) { cout << "[!] City not found" << endl; return; }
    if (city->isInfected) { cout << "[!] Already infected" << endl; return; }

    city->infectedCount   = city->population;
    city->isInfected      = true;
    city->isFullyInfected = true;

    cout << "Day " << day << ": " << cityName
         << " is patient zero! (" << city->population
         << " people infected)" << endl;

    infectionLog.push_back({{cityName, city->population}});

    // save snapshot after infecting
    history.push_back(takeSnapshot());
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
            if (neighbor->isFullyInfected) continue;

            int spreading = (int)(ratio * road.capacity);
            if (spreading <= 0) continue;
            newInfections[neighbor->name] += spreading;
        }
    }

    vector<pair<string, int>> dailyLog;

    for (auto& entry : newInfections) {
        City* city = graph.findCity(entry.first);
        if (!city) continue;

        int before = city->infectedCount;
        city->infectedCount += entry.second;
        if (city->infectedCount > city->population)
            city->infectedCount = city->population;

        int actualNew = city->infectedCount - before;
        if (actualNew <= 0) continue;

        if (city->infectedCount > 0)                 city->isInfected = true;
        if (city->infectedCount >= city->population) city->isFullyInfected = true;

        cout << "  " << city->name
             << " | +" << actualNew
             << " infected | total: " << city->infectedCount
             << "/" << city->population
             << " (" << (int)city->infectionPercent() << "%)" << endl;

        dailyLog.push_back({city->name, actualNew});
    }

    if (newInfections.empty()) {
        cout << "  No new infections today." << endl;
    }

    infectionLog.push_back(dailyLog);

    // save snapshot after each day
    history.push_back(takeSnapshot());
}

// take a snapshot of current graph state
Snapshot Simulator::takeSnapshot() {
    Snapshot snap;
    snap.day = day;

    for (City* city : graph.cities) {
        CitySnapshot cs;
        cs.infectedCount   = city->infectedCount;
        cs.isInfected      = city->isInfected;
        cs.isFullyInfected = city->isFullyInfected;
        cs.isQuarantined   = city->isQuarantined;
        snap.cities[city->name] = cs;
    }

    // save all road states by ID
    for (City* city : graph.cities) {
        for (Road& road : city->roads) {
            // avoid saving same road twice
            bool alreadySaved = false;
            for (auto& rs : snap.roads) {
                if (rs.id == road.id) { alreadySaved = true; break; }
            }
            if (!alreadySaved) {
                RoadSnapshot rs;
                rs.id       = road.id;
                rs.isClosed = road.isClosed;
                snap.roads.push_back(rs);
            }
        }
    }

    return snap;
}

// restore graph to a previous snapshot
void Simulator::loadSnapshot(const Snapshot& snap) {
    day = snap.day;

    // restore city states
    for (City* city : graph.cities) {
        if (snap.cities.count(city->name)) {
            const CitySnapshot& cs = snap.cities.at(city->name);
            city->infectedCount   = cs.infectedCount;
            city->isInfected      = cs.isInfected;
            city->isFullyInfected = cs.isFullyInfected;
            city->isQuarantined   = cs.isQuarantined;
        }
    }

    // restore road states
    for (City* city : graph.cities) {
        for (Road& road : city->roads) {
            for (const RoadSnapshot& rs : snap.roads) {
                if (rs.id == road.id) {
                    road.isClosed = rs.isClosed;
                    break;
                }
            }
        }
    }
}

void Simulator::printHistory() {
    cout << "\n=== Simulation History ===" << endl;
    if (history.empty()) {
        cout << "[!] No history recorded yet" << endl;
        return;
    }
    for (auto& snap : history) {
        cout << "\nDay " << snap.day << ":" << endl;
        for (auto& entry : snap.cities) {
            const CitySnapshot& cs = entry.second;
            if (cs.infectedCount > 0 || cs.isQuarantined) {
                cout << "  " << entry.first
                     << " | " << cs.infectedCount
                     << (cs.isQuarantined   ? " [QUARANTINED]"   :
                         cs.isFullyInfected ? " [FULLY INFECTED]" :
                         cs.isInfected      ? " [PARTIAL]"        : "")
                     << endl;
            }
        }
    }
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