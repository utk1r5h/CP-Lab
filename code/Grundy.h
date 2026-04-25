#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
#include "Graph.h"
using namespace std;

// represents a snapshot of the game at any point
struct GameState {
    set<string> infected;
    set<string> quarantined;

    // needed to use GameState as map key
    bool operator<(const GameState& other) const {
        if (infected != other.infected) return infected < other.infected;
        return quarantined < other.quarantined;
    }
};

class Grundy {
private:
    Graph& graph;
    map<GameState, int> memo; // memoization cache

    // get all cities virus can spread to from current state
    vector<string> getSpreadTargets(const GameState& state);

    // get all valid quarantine moves government can make
    vector<string> getQuarantineMoves(const GameState& state);

    // spread virus one step, return new state
    GameState spreadVirus(const GameState& state);

    // calculate grundy number for a state
    int calculateGrundy(const GameState& state, int depth);

public:
    Grundy(Graph& g);

    // main function — given current game state, find best quarantine move
    void analyze(const GameState& currentState);

    // build current state from actual simulation
    GameState buildCurrentState();

    // show what happens with vs without optimal quarantine
    void compareStrategies(GameState state);
};