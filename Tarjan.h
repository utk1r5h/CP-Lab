#pragma once
#include <map>
#include "Graph.h"
using namespace std;

class Tarjan {
private:
    Graph& graph;
    map<string, int> discovery;
    map<string, int> low;
    map<string, bool> visited;
    int timer;

    void dfs(City* city, int parentRoadId);

public:
    Tarjan(Graph& g);
    void findCriticalPoints();
};