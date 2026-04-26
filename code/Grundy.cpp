#include "Grundy.h"
#include <iostream>
#include <algorithm>
#include <climits>
using namespace std;

Grundy::Grundy(Graph& g) : graph(g) {}

int Grundy::totalInfected(const GameState& state) const {
    int total = 0;
    for (auto& entry : state.infectedCount)
        total += entry.second;
    return total;
}

int Grundy::totalHealthy(const GameState& state) const {
    int total = 0;
    for (City* city : graph.cities) {
        total += state.healthyPeople(city->name);
    }
    return total;
}

// cities that can receive new infections this step
vector<string> Grundy::getSpreadTargets(const GameState& state) {
    set<string> targets;
    for (City* city : graph.cities) {
        if (!state.isInfected(city->name)) continue;
        if (state.quarantined.count(city->name)) continue;

        for (Road& road : city->roads) {
            if (road.isClosed) continue;
            string neighbor = road.destination->name;
            if (state.quarantined.count(neighbor)) continue;
            // can spread to ANY city not fully infected
            // even partially infected ones get worse
            if (!state.isFullyInfected(neighbor)) {
                targets.insert(neighbor);
            }
        }
    }
    return vector<string>(targets.begin(), targets.end());
}

// KEY CHANGE — city is quarantinable if not fully infected
// and not already quarantined
vector<string> Grundy::getQuarantineMoves(const GameState& state) {
    vector<string> moves;
    for (City* city : graph.cities) {
        if (state.quarantined.count(city->name)) continue;

        // can quarantine if not fully infected
        // this includes healthy cities AND partially infected ones
        if (!state.isFullyInfected(city->name)) {
            moves.push_back(city->name);
        }
    }
    return moves;
}

// proportional spread in game state
GameState Grundy::spreadVirus(const GameState& state) {
    GameState newState = state;

    for (City* city : graph.cities) {
        if (!state.isInfected(city->name)) continue;
        if (state.quarantined.count(city->name)) continue;

        double ratio = state.infectionRatio(city->name);

        for (Road& road : city->roads) {
            if (road.isClosed) continue;
            string neighbor = road.destination->name;
            if (state.quarantined.count(neighbor)) continue;

            int spreading = (int)(ratio * road.capacity);
            if (spreading <= 0) continue;

            // add to neighbor's infected count
            newState.infectedCount[neighbor] += spreading;

            // cap at population
            int pop = newState.population[neighbor];
            if (newState.infectedCount[neighbor] > pop) {
                newState.infectedCount[neighbor] = pop;
            }
        }
    }
    return newState;
}

int Grundy::calculateGrundy(const GameState& state, int depth) {
    if (depth == 0) return 0;
    if (memo.count(state)) return memo[state];

    // check if virus has no more spread targets — government won
    vector<string> targets = getSpreadTargets(state);
    if (targets.empty()) {
        memo[state] = 1;
        return 1;
    }

    // check if no quarantine moves left
    vector<string> moves = getQuarantineMoves(state);
    if (moves.empty()) {
        memo[state] = 0;
        return 0;
    }

    set<int> reachableGrundy;
    for (const string& move : moves) {
        GameState afterQuarantine = state;
        afterQuarantine.quarantined.insert(move);
        GameState afterSpread = spreadVirus(afterQuarantine);
        int g = calculateGrundy(afterSpread, depth - 1);
        reachableGrundy.insert(g);
    }

    int mex = 0;
    while (reachableGrundy.count(mex)) mex++;

    memo[state] = mex;
    return mex;
}

// build current state from actual simulation
GameState Grundy::buildCurrentState() {
    GameState state;
    for (City* city : graph.cities) {
        state.population[city->name]     = city->population;
        state.infectedCount[city->name]  = city->infectedCount;
        if (city->isQuarantined) state.quarantined.insert(city->name);
    }
    return state;
}

void Grundy::analyze(const GameState& currentState) {
    cout << "\n========================================" << endl;
    cout << "       GRUNDY GAME THEORY ANALYSIS      " << endl;
    cout << "========================================" << endl;

    // print current infection state with actual numbers
    cout << "\nCurrent infection state:" << endl;
    for (City* city : graph.cities) {
        int infected = currentState.infectedCount.count(city->name)
                     ? currentState.infectedCount.at(city->name) : 0;
        int pop      = city->population;
        int percent  = pop > 0 ? (int)((double)infected/pop*100) : 0;

        if (currentState.quarantined.count(city->name)) {
            cout << "  [QUARANTINED] " << city->name
                 << " | " << infected << "/" << pop << endl;
        } else if (currentState.isFullyInfected(city->name)) {
            cout << "  [FULLY INFECTED] " << city->name
                 << " | " << infected << "/" << pop << endl;
        } else if (infected > 0) {
            cout << "  [PARTIAL " << percent << "%] " << city->name
                 << " | " << infected << "/" << pop << endl;
        } else {
            cout << "  [HEALTHY] " << city->name
                 << " | " << infected << "/" << pop << endl;
        }
    }

    cout << "\nTotal infected people: " << totalInfected(currentState) << endl;
    cout << "Total healthy people:  " << totalHealthy(currentState) << endl;

    vector<string> moves = getQuarantineMoves(currentState);
    if (moves.empty()) {
        cout << "\n[!] All cities fully infected — no moves left" << endl;
        return;
    }

    cout << "\nEvaluating quarantine options..." << endl;
    cout << "--------------------------------------------" << endl;

    string bestMove    = "";
    int bestGrundy     = -1;
    int bestHealthy    = -1;

    for (const string& move : moves) {
        GameState afterQuarantine = currentState;
        afterQuarantine.quarantined.insert(move);
        GameState afterSpread = spreadVirus(afterQuarantine);

        int grundyVal      = calculateGrundy(afterSpread, 5);
        int healthyPeople  = totalHealthy(afterSpread);
        int cityPercent    = currentState.infectedCount.count(move)
                           ? (int)(currentState.infectionRatio(move) * 100) : 0;

        cout << "  Quarantine " << move
             << " (currently " << cityPercent << "% infected)"
             << " → Grundy = " << grundyVal
             << " | Healthy people after: " << healthyPeople;

        if (grundyVal > 0) cout << " [WINNING]";
        else               cout << " [LOSING]";
        cout << endl;

        if (grundyVal > bestGrundy ||
           (grundyVal == bestGrundy && healthyPeople > bestHealthy)) {
            bestGrundy  = grundyVal;
            bestMove    = move;
            bestHealthy = healthyPeople;
        }
    }

    cout << "\n========================================" << endl;
    if (bestGrundy > 0) {
        cout << "[GRUNDY] Government is in WINNING position!" << endl;
    } else {
        cout << "[GRUNDY] Virus has advantage — damage control mode" << endl;
    }
    cout << "[GRUNDY] Best move → Quarantine " << bestMove << endl;
    cout << "[GRUNDY] Saves approximately " << bestHealthy
         << " healthy people" << endl;
    cout << "========================================" << endl;
}

void Grundy::compareStrategies(GameState state) {
    cout << "\n========================================" << endl;
    cout << "    STRATEGY COMPARISON                 " << endl;
    cout << "    Grundy guided vs No containment     " << endl;
    cout << "========================================" << endl;

    int initialHealthy = totalHealthy(state);

    // WITHOUT strategy — spread freely for 3 days
    GameState noStrategy = state;
    for (int i = 0; i < 3; i++) {
        noStrategy = spreadVirus(noStrategy);
    }
    int healthyNoStrategy = totalHealthy(noStrategy);

    // WITH grundy — find best move, apply, then spread 3 days
    vector<string> moves = getQuarantineMoves(state);
    string bestMove = "";
    int bestGrundy  = -1;

    for (const string& move : moves) {
        GameState afterQ = state;
        afterQ.quarantined.insert(move);
        GameState afterS = spreadVirus(afterQ);
        int g = calculateGrundy(afterS, 4);
        if (g > bestGrundy) { bestGrundy = g; bestMove = move; }
    }

    GameState withStrategy = state;
    if (!bestMove.empty()) withStrategy.quarantined.insert(bestMove);
    for (int i = 0; i < 3; i++) {
        withStrategy = spreadVirus(withStrategy);
    }
    int healthyWithStrategy = totalHealthy(withStrategy);

    cout << "\nStarting healthy population: " << initialHealthy << endl;
    cout << "\nAfter 3 days WITHOUT containment:" << endl;
    cout << "  Healthy people remaining: " << healthyNoStrategy << endl;
    cout << "  People infected: " << (initialHealthy - healthyNoStrategy) << endl;

    cout << "\nAfter 3 days WITH Grundy (quarantine " << bestMove << "):" << endl;
    cout << "  Healthy people remaining: " << healthyWithStrategy << endl;
    cout << "  People infected: " << (initialHealthy - healthyWithStrategy) << endl;

    cout << "\n[RESULT] Grundy strategy saved "
         << (healthyWithStrategy - healthyNoStrategy)
         << " additional people!" << endl;
    cout << "========================================" << endl;
}