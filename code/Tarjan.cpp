#include "Tarjan.h"
#include <iostream>
using namespace std;

Tarjan::Tarjan(Graph& g) : graph(g) {
    timer = 0;
}

void Tarjan::dfs(City* city, int parentRoadId) {
    visited[city->name] = true;
    discovery[city->name] = low[city->name] = timer++;

    int unvisitedNeighbors = 0;

    for (Road& road : city->roads) {
        City* neighbor = road.destination;

        // skip the road we came from using road ID not city name
        if (road.id == parentRoadId) continue;

        if (!visited[neighbor->name]) {
            unvisitedNeighbors++;
            dfs(neighbor, road.id);

            // update low after returning from neighbor
            low[city->name] = min(low[city->name], low[neighbor->name]);

            // ---- BRIDGE CHECK ----
            if (low[neighbor->name] > discovery[city->name]) {
                cout << "[BRIDGE] " << road.roadName
                     << " (" << city->name << " <-> " << neighbor->name << ")"
                     << " is a critical road!" << endl;
            }

            // ---- ARTICULATION POINT CHECK ----
            if (parentRoadId == -1 && unvisitedNeighbors > 1) {
                // root of DFS tree special case
                cout << "[CRITICAL CITY] " << city->name
                     << " is an articulation point!" << endl;
            }
            if (parentRoadId != -1 && low[neighbor->name] >= discovery[city->name]) {
                // non root case
                cout << "[CRITICAL CITY] " << city->name
                     << " is an articulation point!" << endl;
            }

        } else {
            // already visited → update low
            low[city->name] = min(low[city->name], discovery[neighbor->name]);
        }
    }
}

void Tarjan::findCriticalPoints() {
    // initialize all cities
    for (City* city : graph.cities) {
        visited[city->name] = false;
        discovery[city->name] = -1;
        low[city->name] = -1;
    }

    // run DFS (loop handles disconnected graphs too)
    for (City* city : graph.cities) {
        if (!visited[city->name]) {
            dfs(city, -1); // -1 = no parent road
        }
    }
}