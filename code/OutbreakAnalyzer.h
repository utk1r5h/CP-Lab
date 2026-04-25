#pragma once
#include <vector>
#include <string>
using namespace std;

struct OutbreakWindow {
    int startDay;
    int endDay;
    int totalInfections;
    vector<string> cities;
};

class OutbreakAnalyzer {
private:
    vector<vector<string> >& infectionLog;
    int windowSize;

    // find peak single day in a window
    int peakDayInWindow(int start, int end);

public:
    OutbreakAnalyzer(vector<vector<string> >& log, int windowSize = 3);

    // main sliding window analysis
    OutbreakWindow findWorstWindow();

    // find single worst day
    int findWorstSingleDay();

    // print full analysis
    void analyze();
};