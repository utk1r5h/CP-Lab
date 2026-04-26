#pragma once
#include <map>
#include <string>
#include <vector>
using namespace std;

struct CitySnapshot {
    int infectedCount;
    bool isInfected;
    bool isFullyInfected;
    bool isQuarantined;
};

struct RoadSnapshot {
    int id;
    bool isClosed;
};

struct Snapshot {
    int day;
    map<string, CitySnapshot> cities;  // city name → city state
    vector<RoadSnapshot> roads;        // road id → road state
};