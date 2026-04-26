#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

class DSU {
private:
    map<string, string> parent;
    map<string, int> rank;

public:
    void makeSet(const string& city);
    string find(const string& city);
    void unite(const string& cityA, const string& cityB);
    bool connected(const string& cityA, const string& cityB);
    vector<set<string>> getComponents();
    int componentCount();
};