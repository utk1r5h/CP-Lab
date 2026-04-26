#pragma once
#include <vector>
#include <string>
using namespace std;

struct OutbreakWindow {
    int startDay;
    int endDay;
    long long totalPeopleInfected;   // CHANGED — people not city count
    vector<pair<string, int>> cities; // CHANGED — city + people count
};

class OutbreakAnalyzer {
private:
    vector<vector<pair<string, int>>>& infectionLog;  // CHANGED
    int windowSize;

public:
    OutbreakAnalyzer(vector<vector<pair<string, int>>>& log, int windowSize = 3);

    OutbreakWindow findWorstWindow();
    int findWorstSingleDay();
    void analyze();
};