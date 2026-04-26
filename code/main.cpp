#include <iostream>
#include "Graph.h"
#include "Simulator.h"
#include "Tarjan.h"
#include "Grundy.h"
#include "OutbreakAnalyzer.h"
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
    cout << "  10. Outbreak analysis (sliding window)" << endl;
    cout << "  0. Exit" << endl;
    cout << "========================================" << endl;
    cout << "Enter choice: ";
}


void buildNetwork(Graph& graph) {
    graph.addCity("Delhi",    1500000);
    graph.addCity("Mumbai",   2000000);
    graph.addCity("Pune",     1000000);
    graph.addCity("Kanpur",    600000);
    graph.addCity("Lucknow",   800000);
    graph.addCity("Agra",      400000);
    graph.addCity("Varanasi",  300000);
    graph.addCity("Patna",     200000);

    City* Delhi    = graph.findCity("Delhi");
    City* Mumbai   = graph.findCity("Mumbai");
    City* Pune     = graph.findCity("Pune");
    City* Kanpur   = graph.findCity("Kanpur");
    City* Lucknow  = graph.findCity("Lucknow");
    City* Agra     = graph.findCity("Agra");
    City* Varanasi = graph.findCity("Varanasi");
    City* Patna    = graph.findCity("Patna");

    // capacity = people that can travel per day
    graph.addRoad(Delhi,    Mumbai,   "NH-48",       1400, "highway",    50000);
    graph.addRoad(Delhi,    Mumbai,   "ExpressWay",  1450, "expressway", 80000);
    graph.addRoad(Delhi,    Kanpur,   "NH-19",        440, "highway",    30000);
    graph.addRoad(Mumbai,   Pune,     "NH-4",         150, "highway",    40000);
    graph.addRoad(Mumbai,   Pune,     "Expressway2",  160, "expressway", 60000);
    graph.addRoad(Pune,     Kanpur,   "NH-27",        800, "highway",    20000);
    graph.addRoad(Kanpur,   Lucknow,  "NH-25",         80, "highway",    25000);
    graph.addRoad(Lucknow,  Agra,     "NH-19",        350, "highway",    15000);
    graph.addRoad(Lucknow,  Varanasi, "NH-56",        320, "highway",    10000);
    graph.addRoad(Varanasi, Patna,    "NH-31",        250, "highway",     8000);
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
    cout << "Enter city name: ";
    cin >> cityName;

    City* city = graph.findCity(cityName);
    if (!city) { cout << "[!] City not found" << endl; }
    else {
        cout << "Quarantine options:" << endl;
        cout << "  1. Close ALL roads (full quarantine)" << endl;
        cout << "  2. Close ONE specific road (partial)" << endl;
        int qChoice;
        cout << "Enter choice: ";
        cin >> qChoice;

        if (qChoice == 1) {
            sim.quarantineCity(cityName);
        }
        else if (qChoice == 2) {
            cout << "Roads from " << cityName << ":" << endl;
            for (Road& r : city->roads) {
                cout << "  -> " << r.destination->name
                     << " | " << r.roadName
                     << " | capacity: " << r.capacity
                     << " | " << (r.isClosed ? "CLOSED" : "OPEN")
                     << endl;
            }
            string destName, roadName;
            cout << "Enter destination city: ";
            cin >> destName;
            cout << "Enter road name: ";
            cin >> roadName;
            graph.closeRoad(cityName, destName, roadName);
        }
    }
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
        else if (choice == 10) {
        int windowSize;
        cout << "Enter window size (number of days): ";
        cin >> windowSize;
        OutbreakAnalyzer analyzer(sim.infectionLog, windowSize);
        analyzer.analyze();
        }   

        else {
            cout << "Invalid choice, try again." << endl;
        }
    }

    return 0;
}