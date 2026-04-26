#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include "Graph.h"
using namespace std;

// updated GameState — now tracks actual infection counts
struct GameState {
    map<string, int> infectedCount;   // city name → how many infected
    map<string, int> population;      // city name → total population
    set<string> quarantined;

    bool operator<(const GameState& other) const {
        if (infectedCount != other.infectedCount)
            return infectedCount < other.infectedCount;
        if (quarantined != other.quarantined)
            return quarantined < other.quarantined;
        return false;
    }

    // helper — is city fully infected?
    bool isFullyInfected(const string& cityName) const {
        auto it = infectedCount.find(cityName);
        auto pt = population.find(cityName);
        if (it == infectedCount.end() || pt == population.end()) return false;
        return it->second >= pt->second;
    }

    // helper — is city at least partially infected?
    bool isInfected(const string& cityName) const {
        auto it = infectedCount.find(cityName);
        if (it == infectedCount.end()) return false;
        return it->second > 0;
    }

    // helper — infection ratio of a city
    double infectionRatio(const string& cityName) const {
        auto it = infectedCount.find(cityName);
        auto pt = population.find(cityName);
        if (it == infectedCount.end() || pt == population.end()) return 0;
        if (pt->second == 0) return 0;
        return (double)it->second / pt->second;
    }

    // helper — how many people are still healthy in a city
    int healthyPeople(const string& cityName) const {
        auto it = infectedCount.find(cityName);
        auto pt = population.find(cityName);
        if (pt == population.end()) return 0;
        if (it == infectedCount.end()) return pt->second;
        return pt->second - it->second;
    }
};

class Grundy {
private:
    Graph& graph;
    map<GameState, int> memo;

    vector<string> getSpreadTargets(const GameState& state);
    vector<string> getQuarantineMoves(const GameState& state);
    GameState spreadVirus(const GameState& state);
    int calculateGrundy(const GameState& state, int depth);

    // total infected people across all cities
    int totalInfected(const GameState& state) const;

    // total healthy people across all cities
    int totalHealthy(const GameState& state) const;

public:
    Grundy(Graph& g);
    void analyze(const GameState& currentState);
    GameState buildCurrentState();
    void compareStrategies(GameState state);
};