#include "Counterfactual.h"
#include <iostream>
#include <map>
using namespace std;

Counterfactual::Counterfactual(Graph& g, Simulator& sim)
    : graph(g), realSim(sim) {}

void Counterfactual::runForward(Simulator& sim, int days) {
    for (int i = 0; i < days; i++) {
        sim.stepOneDay();
    }
}

long long Counterfactual::countTotalInfected() {
    long long total = 0;
    for (City* city : graph.cities)
        total += city->infectedCount;
    return total;
}

long long Counterfactual::countTotalHealthy() {
    long long total = 0;
    for (City* city : graph.cities)
        total += (city->population - city->infectedCount);
    return total;
}

map<string, int> Counterfactual::captureFinalStates() {
    map<string, int> states;
    for (City* city : graph.cities)
        states[city->name] = city->infectedCount;
    return states;
}

void Counterfactual::showAvailableDays() {
    cout << "\nAvailable snapshots to replay from:" << endl;
    if (realSim.history.empty()) {
        cout << "[!] No history yet — run simulation first" << endl;
        return;
    }
    for (auto& snap : realSim.history) {
        cout << "  Day " << snap.day << " — ";
        int infected = 0;
        for (auto& entry : snap.cities)
            infected += entry.second.infectedCount;
        cout << infected << " people infected at this point" << endl;
    }
}

void Counterfactual::analyze(int replayFromDay,
                              string quarantineCityName,
                              int forwardDays) {
    cout << "\n========================================" << endl;
    cout << "       COUNTERFACTUAL ANALYSIS          " << endl;
    cout << "========================================" << endl;

    // find the snapshot for requested day
    Snapshot* targetSnap = nullptr;
    for (auto& snap : realSim.history) {
        if (snap.day == replayFromDay) {
            targetSnap = &snap;
            break;
        }
    }

    if (!targetSnap) {
        cout << "[!] No snapshot found for Day " << replayFromDay << endl;
        cout << "    Use option 11 to see available days" << endl;
        return;
    }

    // ---- ACTUAL HISTORY RESULT ----
    // load snapshot and run forward WITHOUT extra intervention
    // to get baseline of what actually happened
    realSim.loadSnapshot(*targetSnap);
    cout << "\nBaseline (no new intervention from Day "
         << replayFromDay << "):" << endl;
    cout << "Running " << forwardDays << " days forward..." << endl;

    // suppress output during replay
    // run forward silently by redirecting cout
    for (int i = 0; i < forwardDays; i++) {
        realSim.stepOneDay();
    }

    long long baselineInfected = countTotalInfected();
    long long baselineHealthy  = countTotalHealthy();
    map<string, int> baselineStates = captureFinalStates();

    cout << "  Total infected after " << forwardDays
         << " days: " << baselineInfected << endl;
    cout << "  Total healthy:  " << baselineHealthy << endl;

    // ---- COUNTERFACTUAL RESULT ----
    // reload same snapshot and apply different intervention
    realSim.loadSnapshot(*targetSnap);
    cout << "\nCounterfactual (quarantine " << quarantineCityName
         << " on Day " << replayFromDay << "):" << endl;
    cout << "Running " << forwardDays << " days forward..." << endl;

    realSim.quarantineCity(quarantineCityName);

    for (int i = 0; i < forwardDays; i++) {
        realSim.stepOneDay();
    }

    long long altInfected = countTotalInfected();
    long long altHealthy  = countTotalHealthy();
    map<string, int> altStates = captureFinalStates();

    cout << "  Total infected after " << forwardDays
         << " days: " << altInfected << endl;
    cout << "  Total healthy:  " << altHealthy << endl;

    // ---- COMPARISON ----
    cout << "\n--------------------------------------------" << endl;
    cout << "CITY BY CITY COMPARISON:" << endl;
    cout << "--------------------------------------------" << endl;
    cout << "City        | Actual      | Counterfactual | Difference" << endl;
    cout << "--------------------------------------------" << endl;

    for (City* city : graph.cities) {
        int actual = baselineStates.count(city->name)
                   ? baselineStates[city->name] : 0;
        int alt    = altStates.count(city->name)
                   ? altStates[city->name] : 0;
        int diff   = actual - alt;

        cout << city->name;
        // padding
        for (int i = city->name.length(); i < 12; i++) cout << " ";
        cout << "| " << actual;
        for (int i = to_string(actual).length(); i < 12; i++) cout << " ";
        cout << "| " << alt;
        for (int i = to_string(alt).length(); i < 15; i++) cout << " ";
        cout << "| ";
        if (diff > 0) cout << "-" << diff << " saved";
        else if (diff < 0) cout << "+" << abs(diff) << " more";
        else cout << "no change";
        cout << endl;
    }

    cout << "\n--------------------------------------------" << endl;
    long long saved = baselineInfected - altInfected;
    if (saved > 0) {
        cout << "[RESULT] Earlier intervention saved "
             << saved << " people!" << endl;
        cout << "[RESULT] Quarantining " << quarantineCityName
             << " on Day " << replayFromDay
             << " instead of later would have helped." << endl;
    } else if (saved < 0) {
        cout << "[RESULT] This intervention made things worse by "
             << abs(saved) << " people." << endl;
        cout << "[RESULT] Original strategy was better." << endl;
    } else {
        cout << "[RESULT] No significant difference." << endl;
    }
    cout << "========================================" << endl;

    // restore simulation to latest state
    if (!realSim.history.empty()) {
        realSim.loadSnapshot(realSim.history.back());
    }
}

void Counterfactual::compareInterventions(int replayFromDay,
                                           vector<string> citiesToTry,
                                           int forwardDays) {
    cout << "\n========================================" << endl;
    cout << "    COMPARING MULTIPLE INTERVENTIONS    " << endl;
    cout << "    Replaying from Day " << replayFromDay << endl;
    cout << "========================================" << endl;

    Snapshot* targetSnap = nullptr;
    for (auto& snap : realSim.history) {
        if (snap.day == replayFromDay) {
            targetSnap = &snap;
            break;
        }
    }

    if (!targetSnap) {
        cout << "[!] No snapshot for Day " << replayFromDay << endl;
        return;
    }

    // baseline first
    realSim.loadSnapshot(*targetSnap);
    for (int i = 0; i < forwardDays; i++) realSim.stepOneDay();
    long long baselineInfected = countTotalInfected();

    cout << "\nBaseline (no intervention): "
         << baselineInfected << " infected" << endl;
    cout << "--------------------------------------------" << endl;

    string bestCity = "";
    long long bestResult = baselineInfected;

    for (const string& cityName : citiesToTry) {
        realSim.loadSnapshot(*targetSnap);
        realSim.quarantineCity(cityName);
        for (int i = 0; i < forwardDays; i++) realSim.stepOneDay();

        long long infected = countTotalInfected();
        long long saved    = baselineInfected - infected;

        cout << "Quarantine " << cityName << " on Day " << replayFromDay
             << " → " << infected << " infected";
        if (saved > 0) cout << " | saves " << saved << " people ✓";
        else           cout << " | no improvement";
        cout << endl;

        if (infected < bestResult) {
            bestResult = infected;
            bestCity   = cityName;
        }
    }

    cout << "\n--------------------------------------------" << endl;
    if (!bestCity.empty()) {
        cout << "[BEST INTERVENTION] Quarantine " << bestCity
             << " on Day " << replayFromDay << endl;
        cout << "[SAVES] " << (baselineInfected - bestResult)
             << " people compared to no intervention" << endl;
    }
    cout << "========================================" << endl;

    // restore to latest
    if (!realSim.history.empty()) {
        realSim.loadSnapshot(realSim.history.back());
    }
}