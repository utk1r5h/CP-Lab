#pragma once
#include "Graph.h"
#include "Snapshot.h"

#include <vector>
#include <string>
#include <map>
using namespace std;

class Simulator {
private:
    Graph& graph;
    int day;

public:
    Simulator(Graph& g);

   vector<vector<pair<string, int>>> infectionLog;
   vector<Snapshot> history;

    void infectCity(string cityName);        // infect patient zero
    void quarantineCity(string cityName);    // block a city
    void stepOneDay();                       // spread infection by 1 day
    void printStatus();  

    Snapshot takeSnapshot();
    void loadSnapshot(const Snapshot& snap);
    void printHistory();                  // print current state

    vector<City*> getInfectedCities();
    vector<City*> getHealthyCities();
     int getDay() { return day; }
};