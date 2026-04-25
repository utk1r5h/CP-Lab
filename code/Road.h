#pragma once
#include <string>
using namespace std;

class City; // forward declaration

class Road {
public:
    int id;
    City* destination;
    string roadName;
    int distance;
    string type;

    Road(int id, City* dest, string roadName, int distance, string type);
};