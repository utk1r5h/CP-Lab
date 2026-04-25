#include "OutbreakAnalyzer.h"
#include <iostream>
#include <algorithm>
using namespace std;

OutbreakAnalyzer::OutbreakAnalyzer(vector<vector<string>>& log, int ws)
    : infectionLog(log), windowSize(ws) {}

int OutbreakAnalyzer::peakDayInWindow(int start, int end) {
    int peakDay = start;
    int peakCount = 0;
    for (int i = start; i <= end; i++) {
        int count = infectionLog[i].size();
        if (count > peakCount) {
            peakCount = count;
            peakDay = i;
        }
    }
    return peakDay;
}

OutbreakWindow OutbreakAnalyzer::findWorstWindow() {
    int n = infectionLog.size();
    OutbreakWindow worst;
    worst.startDay = 0;
    worst.endDay = 0;
    worst.totalInfections = 0;

    if (n == 0) return worst;

    // if simulation ran fewer days than window size
    // just use however many days we have
    int actualWindow = min(windowSize, n);

    // calculate first window
    int currentTotal = 0;
    vector<string> currentCities;

    for (int i = 0; i < actualWindow; i++) {
        for (const string& city : infectionLog[i]) {
            currentTotal++;
            currentCities.push_back(city);
        }
    }

    worst.startDay       = 1; // days are 1-indexed for display
    worst.endDay         = actualWindow;
    worst.totalInfections = currentTotal;
    worst.cities         = currentCities;

    // slide window forward
    for (int i = actualWindow; i < n; i++) {
        // remove leftmost day from window
        int removeDay = i - actualWindow;
        currentTotal -= infectionLog[removeDay].size();

        // remove those cities from currentCities
        for (const string& city : infectionLog[removeDay]) {
            currentCities.erase(
                remove(currentCities.begin(), currentCities.end(), city),
                currentCities.end()
            );
        }

        // add new rightmost day to window
        for (const string& city : infectionLog[i]) {
            currentTotal++;
            currentCities.push_back(city);
        }

        // check if this window is worse
        if (currentTotal > worst.totalInfections) {
            worst.startDay        = removeDay + 2; // 1-indexed
            worst.endDay          = i + 1;
            worst.totalInfections = currentTotal;
            worst.cities          = currentCities;
        }
    }

    return worst;
}

int OutbreakAnalyzer::findWorstSingleDay() {
    int worstDay  = 0;
    int worstCount = 0;

    for (int i = 0; i < (int)infectionLog.size(); i++) {
        int count = infectionLog[i].size();
        if (count > worstCount) {
            worstCount = count;
            worstDay   = i;
        }
    }
    return worstDay; // 0-indexed, add 1 for display
}

void OutbreakAnalyzer::analyze() {
    cout << "\n========================================" << endl;
    cout << "   OUTBREAK ANALYSIS (Window = "
         << windowSize << " days)        " << endl;
    cout << "========================================" << endl;

    if (infectionLog.empty()) {
        cout << "[!] No infection history yet." << endl;
        cout << "    Run simulation first (step some days)" << endl;
        return;
    }

    // print full day by day log first
    cout << "\nDay by day infection log:" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < (int)infectionLog.size(); i++) {
        cout << "  Day " << (i+1) << " → "
             << infectionLog[i].size() << " new infections";
        if (!infectionLog[i].empty()) {
            cout << " : ";
            for (int j = 0; j < (int)infectionLog[i].size(); j++) {
                cout << infectionLog[i][j];
                if (j < (int)infectionLog[i].size()-1) cout << ", ";
            }
        }
        cout << endl;
    }

    // sliding window analysis
    cout << "\nSliding window analysis:" << endl;
    cout << "--------------------------------------------" << endl;

    int n = infectionLog.size();
    int actualWindow = min(windowSize, n);

    // print every window
    for (int i = 0; i <= n - actualWindow; i++) {
        int total = 0;
        for (int j = i; j < i + actualWindow; j++) {
            total += infectionLog[j].size();
        }
        cout << "  Day " << (i+1) << " to Day " << (i+actualWindow)
             << " → " << total << " infections" << endl;
    }

    // worst window result
    OutbreakWindow worst = findWorstWindow();
    cout << "\n--------------------------------------------" << endl;
    cout << "WORST OUTBREAK WINDOW:" << endl;
    cout << "  Days " << worst.startDay << " to " << worst.endDay << endl;
    cout << "  Total new infections: " << worst.totalInfections << endl;
    cout << "  Cities infected in this window:" << endl;
    for (const string& city : worst.cities) {
        cout << "    -> " << city << endl;
    }

    // worst single day
    int worstDay = findWorstSingleDay();
    cout << "\nWORST SINGLE DAY:" << endl;
    cout << "  Day " << (worstDay+1) << " with "
         << infectionLog[worstDay].size() << " new infections" << endl;

    cout << "========================================" << endl;
}