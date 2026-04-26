#pragma once
#include "Simulator.h"
#include "Graph.h"
#include "Snapshot.h"
#include <string>
#include <vector>
using namespace std;

struct CounterfactualResult {
    int replayFromDay;
    string intervention;       // what quarantine was applied
    int interventionDay;       // when it was applied
    long long totalInfected;   // total infected at end
    long long totalHealthy;    // total healthy at end
    map<string, int> finalCityStates; // city → infected count
};

class Counterfactual {
private:
    Graph& graph;
    Simulator& realSim;        // the actual simulation that ran

    // run simulation forward from current state for N days
    void runForward(Simulator& sim, int days);

    // count total infected across all cities
    long long countTotalInfected();

    // count total healthy across all cities
    long long countTotalHealthy();

    // capture final city states
    map<string, int> captureFinalStates();

public:
    Counterfactual(Graph& g, Simulator& sim);

    // main function — replay from a day with different quarantine
    void analyze(int replayFromDay, string quarantineCity, int forwardDays);

    // compare multiple interventions from same day
    void compareInterventions(int replayFromDay, 
                               vector<string> citiesToTry, 
                               int forwardDays);

    // show available snapshots
    void showAvailableDays();
};