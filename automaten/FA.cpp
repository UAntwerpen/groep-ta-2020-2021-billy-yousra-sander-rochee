//
// Created by sande on 10/03/2021.
//

#include "FA.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

State::State(const State &state) {
    name = state.name;
    starting = state.starting;
    accepting = state.accepting;
    transitions = state.transitions;
}

State & State::operator=(const State &copy) {
    name = copy.name;
    starting = copy.starting;
    accepting = copy.accepting;
    for (const auto& pair : copy.transitions) {
        transitions[pair.first] = pair.second;
    }
    return *this;
}

deadState::deadState(vector<char> &alphabet) {
    name = "";
    // Set transitions for whole alphabet to self
    for (auto c : alphabet) {
        transitions.insert({c, {this}});
    }
}

FA::FA(const string &fileName) {
    // Open and read json file
    ifstream in(fileName);
    json j;
    in >> j;

    type = j["type"]; // Get type

    // Create alphabet
    for (string c : j["alphabet"]) {
        alphabet.push_back(c[0]);
    }
    // Initialise states
    for (auto state : j["states"]) {
        State* s = new State;
        s->name = state["name"];
        // Accepting state
        if (state["accepting"]) {
            // Add to the accepting states vector
            acceptingStates.push_back(s);
            s->accepting = true;
        }
        // Starting state
        if (state["starting"]) {
            // Set the start state
            startState = s;
            s->starting = true;
        }
        // Store state
        states.push_back(s);
//        cout << "New State: " << s->name << endl;
    }
    // Initialise state transitions
    for (auto trans : j["transitions"]) {
        State* sPtr = nullptr;
        State* ePtr = nullptr;
        // Get correct states for transition
        for (auto state : states) {
            // Check whether names correspond
            if (state->name == trans["from"]) sPtr = state; // Set the right pointer
            if (state->name == trans["to"]) ePtr = state;
        }
        // Get transition character
        string symb = trans["input"];
        bool inAlpha = false;
        // Check whether transition character in alphabet
        for (char c : alphabet) {
            if (symb[0] == c) { inAlpha = true; break; }
        }
        if (!inAlpha) { // Transition character not in alphabet
            cerr << "Transition on character '" << symb << "' not in alphabet" << endl;
            continue;
        }
        if (sPtr->transitions.find(symb[0]) == sPtr->transitions.cend()) {
            // No transitions on character exist for begin state
            vector<State*> v; v.push_back(ePtr);
            // Add character and transition state to transition map begin state
            sPtr->transitions.insert({symb[0], v});
        } else {
            // Transitions on character already exist for begin state
            // Add destination state to transition character in transitions of begin state
            sPtr->transitions[symb[0]].push_back(ePtr);
        }
//        cout << "Transition from " << sPtr->name << " to " << ePtr->name << " on " << symb << endl;
    }
}

FA::~FA() {
    // Delete created states
    for (auto state : states) {
        delete state;
    }
}

void FA::clear() {
    alphabet = {};
    acceptingStates = {};
    startState = nullptr;
    type = "FA";
    for (auto sPtr : states) {
        delete sPtr;
    }
    states = {};
}

json FA::createJson() const {
    json j;
    j["type"] = type; // Store type
    j["alphabet"] = alphabet; // Store Alphabet
    // Add every state
    for (State* state : states) {
        // Accepting/starting state bools
        bool accepting = false;
        bool starting = false;
        if (state->accepting) accepting = true;
        if (state->starting) starting = true;
        // Add the states with correct values
        j["states"].push_back({ {"name", state->name}, {"starting", starting}, {"accepting", accepting} });
        // Go over all transition sets of current state
        for (auto trans : state->transitions) {
            // Go over destination states
            for (auto s : trans.second) {
                // Add the transition with correct values to the json
                j["transitions"].push_back({
                                                   {"from",  state->name},
                                                   {"to",    s->name},
                                                   {"input", string(1, trans.first)}
                                           });
            }
        }
    }
    return j;
}

void FA::print() const {
    // Creates the json object
    json j = createJson();
//    j["type"] = type; // Store type
//    j["alphabet"] = alphabet; // Store Alphabet
//    // Add every state
//    for (State* state : states) {
//        // Accepting/starting state bools
//        bool accepting = false;
//        bool starting = false;
//        if (state->accepting) accepting = true;
//        if (state->starting) starting = true;
//        // Add the states with correct values
//        j["states"].push_back({ {"name", state->name}, {"starting", starting}, {"accepting", accepting} });
//        for (auto trans : state->transitions) {
//            for (auto s : trans.second) { // Add the transitions
//                j["transitions"].push_back({
//                                                   {"from",  state->name},
//                                                   {"to",    s->name},
//                                                   {"input", string(1, trans.first)}
//                                           });
//            }
//        }
//    }
    // print the json object
    cout << setw(4) << j << endl;
}
