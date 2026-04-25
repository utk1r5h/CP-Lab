#pragma once
#include "Graph.h"
#include <vector>
using namespace std;

class Simulator {
private:
    Graph& graph;
    int day;

public:
    Simulator(Graph& g);

    void infectCity(string cityName);        // infect patient zero
    void quarantineCity(string cityName);    // block a city
    void stepOneDay();                       // spread infection by 1 day
    void printStatus();                      // print current state

    vector<City*> getInfectedCities();
    vector<City*> getHealthyCities();
};