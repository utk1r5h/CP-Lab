#include "OutbreakAnalyzer.h"
#include <iostream>
#include <algorithm>
#include <map>
using namespace std;

OutbreakAnalyzer::OutbreakAnalyzer(vector<vector<pair<string, int>>>& log, int ws)
    : infectionLog(log), windowSize(ws) {}

// helper — total people infected in one day
static long long dayTotal(const vector<pair<string, int>>& day) {
    long long total = 0;
    for (auto& entry : day) total += entry.second;
    return total;
}

OutbreakWindow OutbreakAnalyzer::findWorstWindow() {
    int n = infectionLog.size();
    OutbreakWindow worst;
    worst.startDay           = 1;
    worst.endDay             = 1;
    worst.totalPeopleInfected = 0;

    if (n == 0) return worst;

    int actualWindow = min(windowSize, n);

    // calculate first window
    long long currentTotal = 0;
    vector<pair<string, int>> currentCities;

    for (int i = 0; i < actualWindow; i++) {
        for (auto& entry : infectionLog[i]) {
            currentTotal += entry.second;
            currentCities.push_back(entry);
        }
    }

    worst.startDay            = 1;
    worst.endDay              = actualWindow;
    worst.totalPeopleInfected = currentTotal;
    worst.cities              = currentCities;

    // slide window
    for (int i = actualWindow; i < n; i++) {
        int removeDay = i - actualWindow;

        // remove leftmost day
        currentTotal -= dayTotal(infectionLog[removeDay]);
        for (auto& entry : infectionLog[removeDay]) {
            currentCities.erase(
                remove(currentCities.begin(), currentCities.end(), entry),
                currentCities.end()
            );
        }

        // add new rightmost day
        for (auto& entry : infectionLog[i]) {
            currentTotal += entry.second;
            currentCities.push_back(entry);
        }

        if (currentTotal > worst.totalPeopleInfected) {
            worst.startDay            = removeDay + 2;
            worst.endDay              = i + 1;
            worst.totalPeopleInfected = currentTotal;
            worst.cities              = currentCities;
        }
    }

    return worst;
}

int OutbreakAnalyzer::findWorstSingleDay() {
    int worstDay   = 0;
    long long worstCount = 0;

    for (int i = 0; i < (int)infectionLog.size(); i++) {
        long long count = dayTotal(infectionLog[i]);
        if (count > worstCount) {
            worstCount = count;
            worstDay   = i;
        }
    }
    return worstDay;
}

void OutbreakAnalyzer::analyze() {
    cout << "\n========================================" << endl;
    cout << "   OUTBREAK ANALYSIS (Window = "
         << windowSize << " days)" << endl;
    cout << "========================================" << endl;

    if (infectionLog.empty()) {
        cout << "[!] No infection history yet." << endl;
        cout << "    Run simulation first (step some days)" << endl;
        return;
    }

    // day by day log
    cout << "\nDay by day infection log:" << endl;
    cout << "--------------------------------------------" << endl;
    for (int i = 0; i < (int)infectionLog.size(); i++) {
        long long total = dayTotal(infectionLog[i]);
        cout << "  Day " << (i+1)
             << " → " << total << " new people infected";

        if (!infectionLog[i].empty()) {
            cout << " | Cities: ";
            for (int j = 0; j < (int)infectionLog[i].size(); j++) {
                cout << infectionLog[i][j].first
                     << " (+" << infectionLog[i][j].second << ")";
                if (j < (int)infectionLog[i].size()-1) cout << ", ";
            }
        }
        cout << endl;
    }

    // sliding window
    cout << "\nSliding window analysis (people infected):" << endl;
    cout << "--------------------------------------------" << endl;

    int n = infectionLog.size();
    int actualWindow = min(windowSize, n);

    for (int i = 0; i <= n - actualWindow; i++) {
        long long total = 0;
        for (int j = i; j < i + actualWindow; j++) {
            total += dayTotal(infectionLog[j]);
        }
        cout << "  Day " << (i+1) << " to Day " << (i+actualWindow)
             << " → " << total << " people infected" << endl;
    }

    // worst window
    OutbreakWindow worst = findWorstWindow();
    cout << "\n--------------------------------------------" << endl;
    cout << "WORST OUTBREAK WINDOW:" << endl;
    cout << "  Days " << worst.startDay << " to " << worst.endDay << endl;
    cout << "  Total people newly infected: "
         << worst.totalPeopleInfected << endl;
    cout << "  Cities affected in this window:" << endl;

    // group by city name to avoid repetition
    map<string, long long> cityTotals;
    for (auto& entry : worst.cities) {
        cityTotals[entry.first] += entry.second;
    }
    for (auto& entry : cityTotals) {
        cout << "    -> " << entry.first
             << " | +" << entry.second << " people" << endl;
    }

    // worst single day
    int worstDay = findWorstSingleDay();
    cout << "\nWORST SINGLE DAY:" << endl;
    cout << "  Day " << (worstDay+1)
         << " with " << dayTotal(infectionLog[worstDay])
         << " new people infected" << endl;

    cout << "========================================" << endl;
}