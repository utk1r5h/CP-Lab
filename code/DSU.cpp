#include "DSU.h"

void DSU::makeSet(const string& city) {
    parent[city] = city;
    rank[city]   = 0;
}

string DSU::find(const string& city) {
    if (parent[city] != city)
        parent[city] = find(parent[city]); // path compression
    return parent[city];
}

void DSU::unite(const string& cityA, const string& cityB) {
    string rootA = find(cityA);
    string rootB = find(cityB);
    if (rootA == rootB) return;

    if (rank[rootA] < rank[rootB])      parent[rootA] = rootB;
    else if (rank[rootA] > rank[rootB]) parent[rootB] = rootA;
    else { parent[rootB] = rootA; rank[rootA]++; }
}

bool DSU::connected(const string& cityA, const string& cityB) {
    return find(cityA) == find(cityB);
}

vector<set<string>> DSU::getComponents() {
    map<string, set<string>> componentMap;
    for (auto& entry : parent) {
        string root = find(entry.first);
        componentMap[root].insert(entry.first);
    }
    vector<set<string>> components;
    for (auto& entry : componentMap)
        components.push_back(entry.second);
    return components;
}

int DSU::componentCount() {
    return getComponents().size();
}