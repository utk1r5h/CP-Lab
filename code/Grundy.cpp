#include "Grundy.h"
#include <iostream>
#include <algorithm>
#include <climits>
using namespace std;

Grundy::Grundy(Graph& g) : graph(g) {}

// update getSpreadTargets to respect closed roads
vector<string> Grundy::getSpreadTargets(const GameState& state) {
    set<string> targets;
    for (City* city : graph.cities) {
        if (state.infected.count(city->name) &&
            !state.quarantined.count(city->name)) {
            for (Road& road : city->roads) {
                if (road.isClosed) continue; // NEW — respect closed roads
                string neighbor = road.destination->name;
                if (!state.infected.count(neighbor) &&
                    !state.quarantined.count(neighbor)) {
                    targets.insert(neighbor);
                }
            }
        }
    }
    return vector<string>(targets.begin(), targets.end());
}

// government can quarantine any healthy uninfected city
vector<string> Grundy::getQuarantineMoves(const GameState& state) {
    vector<string> moves;
    for (City* city : graph.cities) {
        if (!state.infected.count(city->name) &&
            !state.quarantined.count(city->name)) {
            moves.push_back(city->name);
        }
    }
    return moves;
}

// spread virus to all reachable cities, return new state
GameState Grundy::spreadVirus(const GameState& state) {
    GameState newState = state;
    vector<string> targets = getSpreadTargets(state);
    for (const string& t : targets) {
        newState.infected.insert(t);
    }
    return newState;
}

// grundy number calculation with memoization
// depth limits recursion so it doesnt run forever
int Grundy::calculateGrundy(const GameState& state, int depth) {
    if (depth == 0) return 0;

    // check cache
    if (memo.count(state)) return memo[state];

    // check if all cities are infected or quarantined — virus won
    bool anyHealthy = false;
    for (City* city : graph.cities) {
        if (!state.infected.count(city->name) &&
            !state.quarantined.count(city->name)) {
            anyHealthy = true;
            break;
        }
    }
    if (!anyHealthy) {
        memo[state] = 0;
        return 0;
    }

    // check if virus has no more moves — government won
    vector<string> targets = getSpreadTargets(state);
    if (targets.empty()) {
        memo[state] = 1; // government wins
        return 1;
    }

    // government picks a city to quarantine
    // then virus spreads
    // government wants to reach a state where grundy > 0
    vector<string> moves = getQuarantineMoves(state);

    set<int> reachableGrundy;
    for (const string& move : moves) {
        // government quarantines this city
        GameState afterQuarantine = state;
        afterQuarantine.quarantined.insert(move);

        // then virus spreads
        GameState afterSpread = spreadVirus(afterQuarantine);

        int g = calculateGrundy(afterSpread, depth - 1);
        reachableGrundy.insert(g);
    }

    // mex — minimum excludant
    // smallest non-negative integer NOT in reachableGrundy
    int mex = 0;
    while (reachableGrundy.count(mex)) mex++;

    memo[state] = mex;
    return mex;
}

GameState Grundy::buildCurrentState() {
    GameState state;
    for (City* city : graph.cities) {
        if (city->isInfected)    state.infected.insert(city->name);
        if (city->isQuarantined) state.quarantined.insert(city->name);
    }
    return state;
}

void Grundy::analyze(const GameState& currentState) {
    cout << "\n========================================" << endl;
    cout << "       GRUNDY GAME THEORY ANALYSIS      " << endl;
    cout << "========================================" << endl;

    cout << "\nCurrent infected cities:" << endl;
    for (const string& c : currentState.infected)
        cout << "  [INFECTED]  " << c << endl;

    cout << "\nCurrent quarantined cities:" << endl;
    if (currentState.quarantined.empty())
        cout << "  none" << endl;
    for (const string& c : currentState.quarantined)
        cout << "  [QUARANTINE] " << c << endl;

    // find best quarantine move
    vector<string> moves = getQuarantineMoves(currentState);
    if (moves.empty()) {
        cout << "\n[!] No quarantine moves available" << endl;
        return;
    }

    cout << "\nEvaluating all possible quarantine moves..." << endl;
    cout << "--------------------------------------------" << endl;

    string bestMove = "";
    int bestGrundy = -1;
    int bestSaved = -1;

    for (const string& move : moves) {
        GameState afterQuarantine = currentState;
        afterQuarantine.quarantined.insert(move);
        GameState afterSpread = spreadVirus(afterQuarantine);

        int grundyVal = calculateGrundy(afterSpread, 6);

        // count how many cities stay healthy
        int healthy = 0;
        for (City* city : graph.cities) {
            if (!afterSpread.infected.count(city->name) &&
                !afterSpread.quarantined.count(city->name)) {
                healthy++;
            }
        }

        cout << "  Quarantine " << move
             << " → Grundy = " << grundyVal
             << " | Healthy cities after: " << healthy;

        if (grundyVal > 0) cout << " ✓ WINNING";
        else               cout << " ✗ LOSING";
        cout << endl;

        if (grundyVal > bestGrundy ||
           (grundyVal == bestGrundy && healthy > bestSaved)) {
            bestGrundy = grundyVal;
            bestMove   = move;
            bestSaved  = healthy;
        }
    }

    cout << "\n========================================" << endl;
    if (bestGrundy > 0) {
        cout << "[GRUNDY] Government is in WINNING position!" << endl;
        cout << "[GRUNDY] Optimal move → Quarantine " << bestMove << endl;
        cout << "[GRUNDY] This saves " << bestSaved << " cities from infection" << endl;
    } else {
        cout << "[GRUNDY] Virus is in WINNING position!" << endl;
        cout << "[GRUNDY] Best damage control → Quarantine " << bestMove << endl;
        cout << "[GRUNDY] Minimizes spread to " << bestSaved << " healthy cities" << endl;
    }
    cout << "========================================" << endl;
}

void Grundy::compareStrategies(GameState state) {
    cout << "\n========================================" << endl;
    cout << "    STRATEGY COMPARISON                 " << endl;
    cout << "    With Grundy vs Without Grundy       " << endl;
    cout << "========================================" << endl;

    // WITHOUT grundy — just spread freely for 3 days
    GameState noStrategy = state;
    for (int i = 0; i < 3; i++) {
        noStrategy = spreadVirus(noStrategy);
    }

    int infectedNoStrategy = noStrategy.infected.size();

    // WITH grundy — find best move, quarantine, then spread
    vector<string> moves = getQuarantineMoves(state);
    string bestMove = "";
    int bestGrundy = -1;

    for (const string& move : moves) {
        GameState afterQ = state;
        afterQ.quarantined.insert(move);
        GameState afterS = spreadVirus(afterQ);
        int g = calculateGrundy(afterS, 5);
        if (g > bestGrundy) { bestGrundy = g; bestMove = move; }
    }

    GameState withStrategy = state;
    if (!bestMove.empty()) withStrategy.quarantined.insert(bestMove);
    for (int i = 0; i < 3; i++) {
        withStrategy = spreadVirus(withStrategy);
    }

    int infectedWithStrategy = withStrategy.infected.size();
    int citiesSaved = infectedNoStrategy - infectedWithStrategy;

    cout << "\nSimulating 3 days forward from current state..." << endl;
    cout << "\nWithout Grundy (no quarantine):" << endl;
    cout << "  Infected after 3 days: " << infectedNoStrategy << " cities" << endl;
    for (const string& c : noStrategy.infected)
        cout << "    -> " << c << endl;

    cout << "\nWith Grundy (quarantine " << bestMove << "):" << endl;
    cout << "  Infected after 3 days: " << infectedWithStrategy << " cities" << endl;
    for (const string& c : withStrategy.infected)
        cout << "    -> " << c << endl;

    cout << "\n[RESULT] Grundy strategy saved "
         << citiesSaved << " cities!" << endl;
    cout << "========================================" << endl;
}