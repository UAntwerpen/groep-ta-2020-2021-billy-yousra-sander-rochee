//
// Created by sande on 10/03/2021.
//

#include "NFA.h"
#include "DFA.h"
#include <iostream>
#include <algorithm>

/**
 * Constructor for the NFA
 * @param fileName: Name of the file to import
 */
NFA::NFA(const string &fileName) : FA(fileName) {
    // Check for correct input type
    if (type != "NFA")
        cerr << "Wrong type in input file." << endl;
    type = "NFA";
}

/**
 * Checks whether the given string gets accepted by the FA
 * @param inString: Input string
 * @return          Bool whether accepted
 */
bool NFA::accepts(const string &inString) const {
    // Vector to store all current states
    vector<State*> curStates;
    // Start at start state
    curStates.push_back(startState);

    // Go over input string
    for (auto c : inString) {
        // Temporary vector to store destination states
        vector<State*> tempStates;
        // Go over all current states
        for (auto curState : curStates) {
            // Add destination states for transition on character to tempStates vector
            tempStates.insert(end(tempStates), begin(curState->transitions[c]), end(curState->transitions[c]));
        }
        // Update current states to tempStates (destination states)
        curStates = tempStates;
    }
    // Go over current states at end of string
    for (auto s : curStates) {
        if (s->accepting)
            return true; // If one of states is accepting, string gets accepted

//        for (auto fS : acceptingStates){ // Old way of checking)
//            if (s == fS) {
//                return true;
//            }
//        }
    }
    return false;
}

/**
 * Creates combined state from states in given states vector
 * Recursively creates transition states
 * @param stateMap: Map containing already combined states [name: state, ...]
 * @param states:   States to be combined
 * @param dead:     Pointer to deadstate
 * @return          name of combined state
 */
string NFA::createStates(map<string, State*> &stateMap, vector<State*> &states, deadState* dead) {
    bool accepting = false;
    // Sorts the state vector alphabetically
    // https://stackoverflow.com/questions/7803634/sorting-a-vectorstruct-alphabetically
    sort(states.begin(), states.end(), [](const State* lhs, const State* rhs) { return lhs->name < rhs->name; });
    // Create name of new state {state1.name, ...}
    string name = "{";
    for (auto state : states) {
        name.append(state->name + ",");
        // If one of the states is accepting combined state is accepting
        if (state->accepting)
            accepting = state->accepting;
        // Can't check for starting as starting does not work the same as accepting
        // (only a single start state is possible)
    }
    // Replace last ',' in name with '}'
    name.replace(name.size()-1, 1, "}");
    // State not yet created (thus state name not in stateMap)
    if (stateMap.find(name) == stateMap.end()) {
        // Create new state
        State* nState = new State;
        // Set new state parameters
        nState->name = name;
        nState->accepting = accepting;
        stateMap.insert( {name, nState} );
        // Go over alphabet
        for (char c : alphabet) {
            // Vector to add all destination states on transition for alphabet character
            vector<State *> transStates;
            // Go over states in states vector
            for (auto state : states) {
                // Go over transitions for current state and character
                for (auto transition : state->transitions[c]) {
                    // If transition not in transitions vector
                    if (find(transStates.cbegin(), transStates.cend(), transition) == transStates.cend())
                        // add transition state
                        transStates.push_back(transition);
                }
            }
            // No transitions
            if (transStates.empty()) {
                // Transition to dead state
                transStates.push_back(dead);
//                dead->used = true;
            }
            // Create combined state of transition states
            string transStateStr = createStates(stateMap, transStates, dead);
            State* transState = stateMap[transStateStr];
            // Add transition from new state to created combined transition state
            nState->transitions.insert({c, {transState}});
        }
    }
    return name;
}

/**
 * Creates a DFA of the NFA
 * @return
 */
DFA NFA::toDFA() {
    // Create a DFA
    DFA dfa;
    dfa.setAlphabet(alphabet);

    // new dead state to add to empty transitions
    deadState* dead = new deadState(alphabet);

    // Set up parameters to create states
    map<string, State*> stateMap;
    vector<State*> states{startState};
    // Combine the states of the NFA starting from the start state
    string sState = createStates(stateMap, states, dead);
    // Returned state name is starting state
    stateMap[sState]->starting = true;
    // Go over states and add to correct parameter in DFA
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

    // Don't forget to delete the deadstate
    delete dead;
//    }
    return dfa;
}
