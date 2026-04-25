#include <iostream>
#include "Graph.h"
#include "Simulator.h"
#include "Tarjan.h"
#include "Grundy.h"
using namespace std;

void printMenu() {
    cout << "\n========================================" << endl;
    cout << "       EPIDEMIC SPREAD SIMULATOR        " << endl;
    cout << "========================================" << endl;
    cout << "  1. View city network" << endl;
    cout << "  2. Run Tarjan's analysis" << endl;
    cout << "  3. Infect a city (patient zero)" << endl;
    cout << "  4. Step one day" << endl;
    cout << "  5. Quarantine a city" << endl;
    cout << "  6. Run Grundy analysis" << endl;
    cout << "  7. Compare strategies (Grundy vs none)" << endl;
    cout << "  8. Run full auto demo" << endl;
    cout << "  9. View current status" << endl;
    cout << "  0. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}

void buildNetwork(Graph& graph) {
    // 8 cities — designed so Lucknow and Varanasi
    // are clear articulation points
    // Patna only reachable through Varanasi
    // Agra only reachable through Lucknow

    graph.addCity("Delhi",    15000);
    graph.addCity("Mumbai",   20000);
    graph.addCity("Pune",     10000);
    graph.addCity("Kanpur",    6000);
    graph.addCity("Lucknow",   8000);
    graph.addCity("Agra",      4000);
    graph.addCity("Varanasi",  3000);
    graph.addCity("Patna",     2000);

    City* Delhi    = graph.findCity("Delhi");
    City* Mumbai   = graph.findCity("Mumbai");
    City* Pune     = graph.findCity("Pune");
    City* Kanpur   = graph.findCity("Kanpur");
    City* Lucknow  = graph.findCity("Lucknow");
    City* Agra     = graph.findCity("Agra");
    City* Varanasi = graph.findCity("Varanasi");
    City* Patna    = graph.findCity("Patna");

    // Delhi <-> Mumbai (two roads — not a bridge)
    graph.addRoad(Delhi,   Mumbai,   "NH-48",      1400, "highway");
    graph.addRoad(Delhi,   Mumbai,   "ExpressWay", 1450, "expressway");

    // Delhi <-> Kanpur (single — bridge)
    graph.addRoad(Delhi,   Kanpur,   "NH-19",       440, "highway");

    // Mumbai <-> Pune (two roads — not a bridge)
    graph.addRoad(Mumbai,  Pune,     "NH-4",        150, "highway");
    graph.addRoad(Mumbai,  Pune,     "Expressway2", 160, "expressway");

    // Pune <-> Kanpur (single — bridge)
    graph.addRoad(Pune,    Kanpur,   "NH-27",       800, "highway");

    // Kanpur <-> Lucknow (single — bridge)
    // Lucknow is articulation point
    graph.addRoad(Kanpur,  Lucknow,  "NH-25",        80, "highway");

    // Lucknow <-> Agra (single — Agra isolated without Lucknow)
    graph.addRoad(Lucknow, Agra,     "NH-19",       350, "highway");

    // Lucknow <-> Varanasi (single — bridge)
    // Varanasi is also articulation point
    graph.addRoad(Lucknow, Varanasi, "NH-56",       320, "highway");

    // Varanasi <-> Patna (single — Patna isolated without Varanasi)
    graph.addRoad(Varanasi, Patna,   "NH-31",       250, "highway");
}

void runFullDemo(Graph& graph, Simulator& sim) {
    cout << "\n========================================" << endl;
    cout << "         FULL AUTO DEMO                 " << endl;
    cout << "========================================" << endl;

    cout << "\n[STEP 1] Running Tarjan's analysis first..." << endl;
    Tarjan tarjan(graph);
    tarjan.findCriticalPoints();

    cout << "\n[STEP 2] Delhi is patient zero..." << endl;
    sim.infectCity("Delhi");
    sim.printStatus();

    cout << "\n[STEP 3] Spreading for 2 days without containment..." << endl;
    sim.stepOneDay();
    sim.stepOneDay();
    sim.printStatus();

    cout << "\n[STEP 4] Running Grundy analysis..." << endl;
    Grundy grundy(graph);
    GameState current = grundy.buildCurrentState();
    grundy.analyze(current);

    cout << "\n[STEP 5] Applying Grundy recommendation..." << endl;
    cout << "         Quarantining Lucknow..." << endl;
    sim.quarantineCity("Lucknow");

    cout << "\n[STEP 6] Stepping 2 more days with containment..." << endl;
    sim.stepOneDay();
    sim.stepOneDay();
    sim.printStatus();

    cout << "\n[STEP 7] Comparing what would have happened..." << endl;
    GameState stateBeforeQ = grundy.buildCurrentState();
    grundy.compareStrategies(current);
}

int main() {
    Graph graph;
    buildNetwork(graph);
    Simulator sim(graph);

    cout << "\nNetwork built successfully!" << endl;
    cout << "8 cities, multiple roads loaded." << endl;

    int choice;
    while (true) {
        printMenu();
        cin >> choice;

        if (choice == 0) {
            cout << "Exiting simulator." << endl;
            break;
        }

        else if (choice == 1) {
            graph.printGraph();
        }

        else if (choice == 2) {
            Tarjan tarjan(graph);
            tarjan.findCriticalPoints();
        }

        else if (choice == 3) {
            string cityName;
            cout << "Enter city name to infect: ";
            cin >> cityName;
            sim.infectCity(cityName);
        }

        else if (choice == 4) {
            sim.stepOneDay();
        }

        else if (choice == 5) {
            string cityName;
            cout << "Enter city name to quarantine: ";
            cin >> cityName;
            sim.quarantineCity(cityName);
        }

        else if (choice == 6) {
            Grundy grundy(graph);
            GameState current = grundy.buildCurrentState();
            grundy.analyze(current);
        }

        else if (choice == 7) {
            Grundy grundy(graph);
            GameState current = grundy.buildCurrentState();
            grundy.compareStrategies(current);
        }

        else if (choice == 8) {
            runFullDemo(graph, sim);
        }

        else if (choice == 9) {
            sim.printStatus();
        }

        else {
            cout << "Invalid choice, try again." << endl;
        }
    }

    return 0;
}