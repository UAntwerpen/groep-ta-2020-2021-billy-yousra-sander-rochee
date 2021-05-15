//
// Created by sande on 21/03/2021.
//

#include <iostream>
#include <fstream>
#include <algorithm>

#include "../json.hpp"
#include "ENFA.h"
#include "DFA.h"

using json = nlohmann::json;

// Few special cases so FA constructor can't be used
ENFA::ENFA(const string &fileName) {
    // Open and read json file
    ifstream in(fileName);
    json j;
    in >> j;

    type = j["type"]; // Get type
    if (type != "ENFA" && type != "NFA" && type != "DFA")
        cerr << "Wrong type in input file." << endl;

    // Create alphabet
    for (string c : j["alphabet"]) {
        alphabet.push_back(c[0]);
    }
    // Get epsilon character
    string tempEps = j["eps"];
    epsilon = tempEps[0];
    // Initialise states
    for (auto state : j["states"]) {
        State* s = new State;
        s->name = state["name"];
        // Accepting state
        if (state["accepting"]) {
            acceptingStates.push_back(s);
            s->accepting = true;
        }
        // Starting state
        if (state["starting"]) {
            startState = s;
            s->starting = true;
        }
        // Store state
        states.push_back(s);
    }
    // Initialise state transitions
    for (auto trans : j["transitions"]) {
        State* sPtr = nullptr;
        State* ePtr = nullptr;
        // Get correct states for transition
        for (auto state : states) {
            if (state->name == trans["from"]) sPtr = state;
            if (state->name == trans["to"]) ePtr = state;
        }
        string symb = trans["input"];
        bool inAlpha = false;
        // Check whether transition character in alphabet
        for (char c : alphabet) {
            if (symb[0] == c) { inAlpha = true; break; }
        }
        if (!inAlpha && symb != tempEps) { // Transition character not in alphabet
            cerr << "Transition on character '" << symb << "' not in alphabet" << endl;
            continue;
        }
        if (sPtr->transitions.find(symb[0]) == sPtr->transitions.cend()) {
            // No transitions on character exist
            vector<State*> v; v.push_back(ePtr);
            sPtr->transitions.insert({symb[0], v});
        } else {
            // Transitions on character already exist
            sPtr->transitions[symb[0]].push_back(ePtr);
        }
    }
}

void ENFA::printStats() const {
    cout << "no_of_states=" << states.size() << endl;
    string transStr = "no_of_transitions[";
    map<char, int> transCount;
    map<int, int> degree;
    transCount.insert(pair<char, int>(epsilon, 0));
    for (char c : alphabet) {
        transCount.insert(pair<char, int>(c, 0));
    }
    for (auto state : states) {
        int deg = 0;
        for (auto trans : state->transitions) {
            deg += trans.second.size();
            transCount[trans.first] += trans.second.size();
        }
        if (degree.find(deg) != degree.cend())
            degree[deg] += 1;
        else {
            degree.insert(pair<int, int>(deg, 1));
        }
    }
    cout << transStr << epsilon << "]=" << transCount[epsilon] << endl;
    for (auto p : transCount) {
        if (p.first != epsilon)
            cout << transStr << p.first << "]=" << p.second << endl;
    }
    int transC = 0;
    for (auto p : degree) {
        cout << "degree[" << p.first << "]=" << p.second << endl;
        transC += p.first * p.second;
    }
}

vector<State *> ENFA::eClose(State *sState, vector<State *> passed) const {
    vector<State*> eclose = {sState};
    passed.push_back(sState);
    for (auto trans : sState->transitions) {
        if (trans.first == epsilon) {
            for (auto state : trans.second) {
                if (!count(passed.begin(), passed.end(), state)) {
                    passed.push_back(state);
                    vector<State*> ecloseState = eClose(state, passed);
                    eclose.insert(eclose.end(), ecloseState.begin(), ecloseState.end());
                }
            }
        }
    }
    return eclose;
}

/**
 * Checks whether the given string gets accepted by the FA
 * @param inString: Input string
 * @return          Bool whether accepted
 */
bool ENFA::accepts(const string &inString) const {
    // Start with e-closure states of start state
    vector<State*> curStates = eClose(startState);
    // Go over the string
    for (int i = 0; i < inString.size(); i++) {
        char c = inString[i];
        if (!count(alphabet.begin(), alphabet.end(), c))
            c = epsilon;
        vector<State*> newStates;
        // Go over all current states to get transition
        for (auto state : curStates) {
            vector<State*> desStates = state->transitions[c]; // Destination states
            for (State* desState : desStates) {
                if (!count(newStates.begin(), newStates.end(), desState))
                    newStates.push_back(desState);
            }
//            newStates.insert(newStates.end(), desStates.begin(), desStates.end());
            for (auto nState : desStates) {
                vector<State*> ecloseS = eClose(nState);
                for (auto eState : ecloseS) {
                    if (!count(newStates.begin(), newStates.end(), eState))
                        newStates.push_back(eState);
                }
//                newStates.insert(newStates.end(), ecloseS.begin(), ecloseS.end());
            }
        }
        curStates = newStates;
    }
    for (auto state : curStates) {
        if (state->accepting)
            return true;
    }
    return false;
}

string ENFA::createStates(map<string, State *> &stateMap, vector<State *> &states, deadState *dead) {
    bool accepting = false;
    // Sorts the state vector alphabetically
    // https://stackoverflow.com/questions/7803634/sorting-a-vectorstruct-alphabetically
    sort(states.begin(), states.end(), [](const State* lhs, const State* rhs) { return lhs->name < rhs->name; });
    // Create name of new state {states, ...}
    string name = "{";
    for (auto state : states) {
        name.append(state->name + ",");
        if (state->accepting)
            accepting = state->accepting;
    }
    name.replace(name.size()-1, 1, "}");
    // State not yet created
    if (stateMap.find(name) == stateMap.end()) {
        State* nState = new State;
        nState->name = name;
        nState->accepting = accepting;
        stateMap.insert({name, nState});
        for (char c : alphabet) {
            vector<State *> transStates;
            for (auto state : states) {
                vector<State*> transV = state->transitions[c];
                for (auto transS : transV) {
                    vector<State*> eclose = eClose(transS);
                    for (auto eState : eclose) {
                        if (!count(transStates.cbegin(), transStates.cend(), eState))
                            transStates.push_back(eState);
                    }
                }
//                transStates.insert(transStates.cend(), state->transitions[c[0]].cbegin(), state->transitions[c[0]].cend());
            }
            if (transStates.empty()) {
                transStates.push_back(dead);
//                dead->used = true;
            }
            string transStateStr = createStates(stateMap, transStates, dead);
            State* transState = stateMap[transStateStr];
            nState->transitions.insert({c, {transState}});
        }
    }

    return name;
}

DFA ENFA::toDFA() {
    DFA dfa;
    dfa.setAlphabet(alphabet);
    // Dead state for possible dead transitions
    deadState* dead = new deadState(alphabet);
//    for (string c : alphabet) {
//        dead->transitions.insert({c[0], {dead}});
//    }
    map<string, State*> stateMap; // Store all created states
    vector<State*> states = eClose(startState); // Get all states needed for start state
    // Create the states of the dfa
    string sState = createStates(stateMap, states, dead);
    stateMap[sState]->starting = true;
    for (auto set : stateMap) {
        if (set.second->starting)
            dfa.setStart(set.second);
        if (set.second->accepting)
            dfa.addAccepting(set.second);
        dfa.addState(set.second);
    }
//    if (dead->used) {
//        dfa.addState(dead);
//    } else {
    delete dead;
//    }
    return dfa;
}

json ENFA::createJson() const {
    json j = FA::createJson();
    j["eps"] = string(1, epsilon);
    return j;
}


