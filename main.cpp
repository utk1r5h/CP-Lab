#include <iostream>
#include "Graph.h"
#include "Simulator.h"
#include "Tarjan.h"
using namespace std;

int main() {

    // =========================================
    // STEP 1 — BUILD THE CITY NETWORK
    // =========================================

    Graph graph;

    City* Mumbai   = graph.addCity("Mumbai",   20000);
    City* Pune     = graph.addCity("Pune",     10000);
    City* Kanpur   = graph.addCity("Kanpur",   6000);
    City* Lucknow  = graph.addCity("Lucknow",  8000);
    City* Agra     = graph.addCity("Agra",     4000);
    City* Varanasi = graph.addCity("Varanasi", 3000);

    // Mumbai <-> Pune (two roads so NOT a bridge)
    graph.addRoad(Mumbai, Pune,    "NH-48",   150,  "highway");
    graph.addRoad(Mumbai, Pune,    "ExpressWay", 160, "expressway");

    // Pune <-> Kanpur (single road — bridge)
    graph.addRoad(Pune, Kanpur,    "NH-27",   800,  "highway");

    // Kanpur <-> Lucknow (single road — bridge)
    graph.addRoad(Kanpur, Lucknow, "NH-25",   80,   "highway");

    // Lucknow <-> Agra (single — bridge, Agra only via Lucknow)
    graph.addRoad(Lucknow, Agra,   "NH-19",   350,  "highway");

    // Lucknow <-> Varanasi (single — bridge, Varanasi only via Lucknow)
    graph.addRoad(Lucknow, Varanasi, "NH-56", 320,  "highway");

    cout << "========================================" << endl;
    cout << "       EPIDEMIC SPREAD SIMULATOR        " << endl;
    cout << "========================================" << endl;

    // =========================================
    // STEP 2 — TARJAN'S ANALYSIS
    // =========================================

    cout << "\n========================================" << endl;
    cout << "         TARJAN'S ANALYSIS              " << endl;
    cout << "========================================" << endl;

    Tarjan tarjan(graph);
    tarjan.findCriticalPoints();

    cout << "\n[INFO] Lucknow is articulation point"    << endl;
    cout << "[INFO] Agra and Varanasi only reachable"  << endl;
    cout << "[INFO] through Lucknow — quarantine it"   << endl;
    cout << "[INFO] before disease reaches it!"        << endl;

    // =========================================
    // STEP 3 — EPIDEMIC STARTS
    // patient zero = Mumbai
    // =========================================

    cout << "\n========================================" << endl;
    cout << "    EPIDEMIC BEGINS — Patient Zero:     " << endl;
    cout << "    Mumbai                              " << endl;
    cout << "========================================" << endl;

    Simulator sim(graph);
    sim.infectCity("Mumbai");
    sim.printStatus();

    // Day 1 — spreads to Pune
    cout << "\n--- Spreading freely ---" << endl;
    sim.stepOneDay();
    sim.printStatus();

    // Day 2 — spreads to Kanpur
    sim.stepOneDay();
    sim.printStatus();

    // =========================================
    // STEP 4 — APPLY CONTAINMENT on Day 2
    // Tarjan told us Lucknow is critical
    // quarantine it NOW before disease reaches
    // =========================================

    cout << "\n========================================" << endl;
    cout << "       CONTAINMENT STRATEGY             " << endl;
    cout << " Tarjan identified Lucknow as critical! " << endl;
    cout << " Quarantining Lucknow on Day 2...       " << endl;
    cout << " Agra and Varanasi should be saved!     " << endl;
    cout << "========================================" << endl;

    sim.quarantineCity("Lucknow");

    // Day 3 — would have reached Lucknow but it's quarantined
    sim.stepOneDay();
    sim.printStatus();

    // Day 4 — Agra and Varanasi stay healthy
    sim.stepOneDay();
    sim.printStatus();

    // =========================================
    // STEP 5 — FINAL SUMMARY
    // =========================================

    cout << "\n========================================" << endl;
    cout << "           FINAL SUMMARY                " << endl;
    cout << "========================================" << endl;

    vector<City*> infected = sim.getInfectedCities();
    vector<City*> healthy  = sim.getHealthyCities();

    cout << "\nInfected cities (" << infected.size() << "):" << endl;
    for (City* c : infected) {
        cout << "  -> " << c->name << endl;
    }

    cout << "\nHealthy cities (" << healthy.size() << "):" << endl;
    for (City* c : healthy) {
        cout << "  -> " << c->name << endl;
    }

    cout << "\n[CONCLUSION]" << endl;
    cout << "Without Tarjan's — all 6 cities infected" << endl;
    cout << "With Tarjan's    — only 3 cities infected" << endl;
    cout << "Agra and Varanasi saved by quarantining  " << endl;
    cout << "Lucknow (articulation point)!            " << endl;

    return 0;
}