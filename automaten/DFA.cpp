//
// Created by sande on 13/02/2021.
//

#include "DFA.h"
#include "../json.hpp"
#include "RE.h"

#include <string>
#include <map>
#include <iostream>
#include <algorithm>
#include <set>
#include <iostream>

using json = nlohmann::json;


//
// Constructors and destructor
//
DFA::DFA(const string &fileName) : FA(fileName) {
    if (type != "DFA") {
        cerr << "Wrong type in input file." << endl;
    }
    for (auto state : states) {
        vector<string> charCheck; // Checks transition characters
        for (auto trans : state->transitions) {
            charCheck.push_back(to_string(trans.first));
            if (trans.second.size() > 1) { // More than 1 transition on character
                cerr << "Multiple transitions on same input '" << trans.first << "' for state '" << state->name << "' in DFA\n";
            }
        }
        // Check for transitions over every alphabet character
        if (charCheck.size() != alphabet.size()) {
            cerr << "No transition on every alphabet character for state '" << state->name << "' in DFA" << endl;
        }
    }
}

/**
 * Product DFA constructor
 * @param dfa1: 1st DFA of the product
 * @param dfa2: 2nd DFA of the product
 * @param doorsnede:    Bool whether product using union or intersection
 */
DFA::DFA(DFA &dfa1, DFA &dfa2, bool doorsnede) {
    // Set type and alphabet
    type = "DFA";
    // We assume that both DFA's have the same alphabet
    // Otherwise we will have a problem
    alphabet = dfa1.alphabet;

    map<string, State*> stateMap;
    // Create product states starting from the two starting states
    string sState = createProductStates(stateMap, dfa1.startState, dfa2.startState, doorsnede);
    // Set starting true for returned state (start state)
    stateMap[sState]->starting = true;
    // Go over states
    for (auto set : stateMap) {
        // Add state to correct parameters of the DFA
        if (set.second->starting)
            startState = set.second;
        if (set.second->accepting)
            acceptingStates.push_back(set.second);
        states.push_back(set.second);
    }
}

/**
 * Copy constructor
 * @param dfa:  DFA to be copied
 */
DFA::DFA(const DFA &dfa) {
    // Copy alphabet and type
    alphabet = dfa.alphabet;
    type = dfa.type;
    // Go over states in original DFA
    for (auto state : dfa.states) {
        // Create new state
        State* nState = new State;
        // Copy parameters from state in original DFA
        *(nState) = *(state);
        states.push_back(nState); // Add new state to new DFA
    }
    // Loop over states
    for (auto fState : states) {
        // Second loop over states
        for (auto sState : states) {
            // Loop over alphabet
            for (auto c : alphabet) {
                // Check whether transition from first state to second state on current character
                if (sState->transitions[c][0]->name == fState->name)
                    // Update pointer in the transition
                    sState->transitions[c][0] = fState;
            }
        }
        // Add state to correct parameters if necessary
        if (fState->starting)
            startState = fState;
        if (fState->accepting)
            acceptingStates.push_back(fState);
    }
}

//
// Private Methods
//

/**
 * Recursivly creates product states of the DFA starting from two given states
 * Uses lazy evaluation
 * @param stateMap:     Map to store already created states
 * @param st1:          First state
 * @param st2:          Second state
 * @param doorsnede:    Bool whether product using union or intersection
 * @return
 */
string DFA::createProductStates(map<string, State *> &stateMap, State* st1, State* st2, bool doorsnede) {
    bool accepting = false;

    // Create name of new state (state 1,state 2)
    string name = "(" + st1->name + ',' + st2->name + ')';
    // Check whether new state should be accepting
    if ((st1->accepting && st2->accepting && doorsnede) || ((st1->accepting || st2->accepting) && !doorsnede))
        accepting = true;

    // State not yet created
    if (stateMap.find(name) == stateMap.end()) {
        // New state
        State* nState = new State;
        // Set parameters of state
        nState->name = name;
        nState->accepting = accepting;
        // Add to map
        stateMap.insert({name, nState});
        // Loop over the alphabet
        for (char c : alphabet) {
            // Get transitions from both states
            State* trans1 = st1->transitions[c][0];
            State* trans2 = st2->transitions[c][0];
            // Create product state of transition states
            string transStateStr = createProductStates(stateMap, trans1, trans2, doorsnede);
            State* transState = stateMap[transStateStr];
            // Add transition to new transition state
            nState->transitions.insert({c, {transState}});
        }
    }
    // Return name of new state
    return name;
}

/**
 * Alphabetically sort the given state vector
 * @param states:   Vector to be sorted
 * @return          Sorted vector
 */
vector<State *> DFA::sortStates(const vector<State*>& states) const {
    vector<State*> sorted; // Sorted part
    vector<State*> unsorted = states; // Unsorted part
    // While not everything sorted
    while (sorted.size() != states.size()) {
        // Set lowest state to first state
        State* min = unsorted[0];
        int index = 0;
        // Loop over unsorted states
        for (int i = 0; i < unsorted.size(); ++i) {
            // States comes before current min state
            if (unsorted[i]->name < min->name) {
                // Change min state
                min = unsorted[i];
                index = i;
            }
        }
        // Add min state to sorted vector
        sorted.push_back(min);
        // Clear min state from unsorted vector
        unsorted.erase(unsorted.begin() + index);
    }
    return sorted;
}

/**
 * Creates the transition table of the given states
 * { ( state_name, { trans_alpabet[0], trans_alpabet[1], ... } ), ... }
 * @param states:   States of which to create table
 * @return          the table
 */
vector<pair<string, vector<string>>> DFA::createTransTable(const vector<State*>& states) const {
    // Vector to store the table
    vector<pair<string, vector<string>>> transitions;
    // Go over the states
    for (auto state : states) {
        // Vector to store the destination states on transition
        vector<string> transStates;
        // Loop over alphabet
        for (char c : alphabet) {
            // Add transition states to transStates
            if (state->transitions.find(c) != state->transitions.end()) {
                transStates.push_back(state->transitions[c][0]->name);
            }
        }
        // Add state and transitions to table
        transitions.push_back(pair<string, vector<string>>(state->name, transStates));
    }
    return transitions;
}

/**
 * Setup of the table for TFA
 * @param states:   States of which to make table
 * @return          An empty TFA table (empty as in starting table)
 */
vector<vector<string>> DFA::initialiseTable(const vector<State*>& states) const {
    vector<vector<string>> table;
    for (int i = 0; i < states.size(); ++i) {
        vector<string> row;
        for (int j = 0; j <= i+1 && j < states.size(); ++j) {
            row.push_back("");
            // Put names of states in first collum and last row
            // Leave first or last state depending on filling collum or row
            if (j == 0 && i != states.size()-1) {
                row[j] = states[i+1]->name;
            } else if (i == states.size()-1 && j != 0) {
                row[j] = states[j-1]->name;
            } else if (j > 0 && i < (states.size()-1)) {
                // Add marker to collums/rows of accepting states
                if (states[i+1]->accepting && !states[j-1]->accepting)
                    row[j] = "X";
                else if (!states[i+1]->accepting && states[j-1]->accepting)
                    row[j] = "X";
                    // Else nothing thus "-"
                else {
                    row[j] = "-";
                }
            }
        }
        table.push_back(row);
    }
    return table;
}

/**
 * Creates a filled TFA table
 * @param states:   States of which to create the table
 * @return          The TFA table
 */
vector<vector<string>> DFA::createTable(const vector<State*>& states) const {
    vector<State*> sorted = sortStates(states);
    // Get transition table
    vector<pair<string, vector<string>>> transitions = createTransTable(sorted);
    // Get starting table
    vector<vector<string>> table = initialiseTable(sorted);

    // Check to see if new mark added
    bool newMark = true;
    while (newMark) {
        newMark = false;
        // Loop over the table
        for (int i = 0; i < table.size()-1; i++) {
            for (int j = 1; j < table[i].size(); j++) {
                // If spot has a mark
                if (table[i][j] == "X") {
                    // Get states of row and collum
                    pair<string, string> p(table[i][0], table[table.size()-1][j]);
                    // Loop over alphabet
                    for (int k = 0; k < alphabet.size(); ++k) {
                        // Dubble loop through transition table
                        for (auto p1 : transitions) {
                            for (auto p2 : transitions) {
                                // Check whether both points in loop contain names from set p (states of mark)
                                if (p1 != p2 && ((p1.second[k] == p.first && p2.second[k] == p.second) ||
                                                 (p1.second[k] == p.second && p2.second[k] == p.first))) {
                                    // Look for new found position in table
                                    for (int row = 0; row < table.size()-1; row++) {
                                        // Keeps track whether position is found (signle position to mark in table)
                                        bool marked = false;
                                        for (int col = 1; col < table[row].size(); col++) {
                                            if ((table[row][0] == p1.first && table[table.size()-1][col] == p2.first) ||
                                                (table[row][0] == p2.first && table[table.size()-1][col] == p1.first)) {
                                                // Only enable newMark if actual new mark
                                                if (table[row][col] == "-")
                                                    newMark = true;
                                                table[row][col] = "X";
                                                // Position found thus break and enable marked
                                                marked = true;
                                                break;
                                            }
                                        }
                                        // If position found break loop
                                        // Optimizes so that not whole table is checked for a single mark
                                        if (marked)
                                            break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return table;
}

/**
 * Gets al the equivalent states of the DFA
 * @return  Vector containing sets of equivalent states
 */
vector<set<string>> DFA::getEqvStates() const {
    // Create the table
    vector<vector<string>> table = createTable(states);
    vector<set<string>> eqvStates;
    // Loop over rows of the table
    for (int i = 0; i < table.size()-1; ++i) {
        // Loop over collums of the table
        for (int j = 1; j < table[i].size(); ++j) {
            // Empty spot
            if (table[i][j] == "-") {
                // Check to see if states are added to existing set
                bool added = false;
                // Go over sets of equivalent states
                for (auto& set : eqvStates) {
                    // Set containing one of the states exists
                    if (set.find(table[i][0]) != set.cend() || set.find(table[table.size()-1][j]) != set.cend()) {
                        // Add the states to the set
                        set.insert(table[i][0]);
                        set.insert(table[table.size()-1][j]);
                        added = true;
                    }
                }
                // States not added to existing set
                if (!added) {
                    // Create new set of states
                    eqvStates.push_back(set<string>{table[i][0], table[table.size()-1][j]});
                }
            }
        }
    }
    return eqvStates;
}

/**
 * Creates minimized states
 * @param stateNames: Names of states to minimize
 */
void DFA::createMinimizedState(set<string> &stateNames) {
    // Create name of new state
    string nName = "{";
    for (auto s : stateNames) {
        nName.append(s + ", ");
    }
    nName.replace(nName.size()-2, 2, "}");
    // Vector to keep track of devided states
    vector<State*> minStates;
    vector<State*> newStates;
    vector<State*> newAccepting;
    // Create the new state
    State* nState = new State;
    nState->name = nName;

    // Extract bool values and split states in equivalent needed states and other states
    for (auto state : states) {
        // If one of the to be merged states
        if (stateNames.find(state->name) != stateNames.cend()) {
            minStates.push_back(state);
            if (state->accepting)
                nState->accepting = true;
            if (state->starting)
                nState->starting = true;
        }
        // Unchanged state
        else {
            newStates.push_back(state);
            if (state->starting)
                startState = state;
            if (state->accepting)
                newAccepting.push_back(state);
        }
    }
    // Move over not equivalent states and change transitions if necessary
    for (auto state : newStates) {
        for (auto c : alphabet) {
            // Transitions to one of the equivalent states
            if (find(minStates.cbegin(), minStates.cend(), state->transitions[c][0]) != minStates.cend())
                // Change to new state
                state->transitions[c] = vector<State*>{nState};
        }
    }
    // Set transitions for minimized state
    for (auto c : alphabet) {
        State* transS = minStates[0]->transitions[c][0];
        // Transitions to itself
        if (find(minStates.cbegin(), minStates.cend(), transS) != minStates.cend())
            nState->transitions.insert(make_pair(c, vector<State*>{nState}));
        else
            nState->transitions.insert(make_pair(c, vector<State*>{transS}));
    }
    // Add the state to the correct places
    newStates.push_back(nState);
    if (nState->starting)
        startState = nState;
    if (nState->accepting)
        newAccepting.push_back(nState);
    states = newStates;
    acceptingStates = newAccepting;
    // Delete original minimized states
    for (auto state : minStates)
        delete state;
}

//
// Public Methods
//

/**
 * Checks if string gets accepted in DFA
 * @param inString: String to check
 * @return          Bool whether accepted
 */
bool DFA::accepts(const string &inString) const {
    State* curState = startState;
    for (char c : inString) { // Go over characters
        curState = curState->transitions[c][0]; // Get next state
    }
    for (auto state : acceptingStates) { // Check if accepting state
        if (curState == state) return true;
    }
    return false;
}

/**
 * Creates a minimized version of the DFA
 * @return Minimized DFA
 */
DFA DFA::minimize() {
    DFA dfa = *(this);
    vector<set<string>> eqvStates = dfa.getEqvStates();
    for (auto& s : eqvStates) {
        dfa.createMinimizedState(s);
    }
    for (auto s : dfa.states)
        if (s->name[0] != '{')
            s->name = "{" + s->name + "}";
    return dfa;
}

/**
 * Prints the TFA table to the console
 */
void DFA::printTable() const {
    // Get table
    vector<vector<string>> table = createTable(states);
    // Loop over table to print the table
    for (int i = 0; i < table.size(); ++i) {
        for (int j = 0; j <= i+1 && j < table.size(); ++j) {
            cout << table[i][j] << '\t';
        }
        cout << endl;
    }
}

/**
 * Checks whether DFA is equivalent to given DFA
 * @param check Given DFA
 * @return bool whether equivalent
 */
bool DFA::operator==(DFA &check) {
    // States of left DFA
    vector<State*> allStates = sortStates(states);
    // States of right DFA
    vector<State*> compStates = sortStates(check.states);
    // Combine states
    allStates.insert(allStates.cend(), compStates.cbegin(), compStates.cend());

    // Create table of combined states
    vector<vector<string>> table = createTable(allStates);
    bool eqv = false;
    int maxRow = table.size()-1;
    // Loop over table
    for (int i = 0; i < table.size(); ++i) {
        for (int j = 0; j < table[i].size(); ++j) {
            cout << table[i][j] << '\t';
            // If row and collum state are both start states
            if ((table[i][0] == startState->name && table[maxRow][j] == check.startState->name) ||
                    (table[i][0] == check.startState->name && table[maxRow][j] == startState->name))
                // Check whether equivalent states (empty spot)
                eqv = (table[i][j] == "-");
        }
        cout << endl;
    }
    return eqv;
}

//struct reTransition{
//    reTransition(State* s, State* e, string regex) : from(s), to(e), re(regex) {}
//    State* from;
//    State* to;
//    string re;
//};

/**
 * Create a regular expresion from the DFA
 * @return  RE created from DFA
 */
RE DFA::toRE() {
    string regexString;

    for (auto accState : acceptingStates) {
        // Vector to store all states
        vector<State*> elimStates = sortStates(states);
        map<State*, map<State*, string>> transitionMap;
        for (auto state : states) {
            map<State*, string> desMap;
            for (auto trans : state->transitions) {
                string c; c = trans.first;
                for (auto desState : trans.second) {
//                if (state == desState)
//                    c += '*';
                    if (desMap.find(desState) == desMap.cend())
                        desMap.insert(make_pair(desState, c));
                    else
                        desMap[desState] = '(' + desMap[desState] + ")+(" + c + ')';
                }
            }
            if (!desMap.empty())
                transitionMap.insert(make_pair(state, desMap));
        }

        bool onlyAccepting = false;

        while (!onlyAccepting) {
            onlyAccepting = true;
            for (auto state : elimStates) {
                if ((state != startState) && (state != accState))
                    onlyAccepting = false;
            }
            if (onlyAccepting)
                break;

            int i = 0;
            while (elimStates[i] == startState || elimStates[i] == accState) {
                i += 1;
            }

            bool selfTrans = false;
            string selfTransStr;
            State *elimState = elimStates[i];
            if (transitionMap[elimState].find(elimState) != transitionMap[elimState].cend()) {
                selfTrans = true;
                selfTransStr = transitionMap[elimState][elimState];
            }

            map<State *, map<State *, string>> tempMap;

            for (auto set : transitionMap) {
                if (set.first == elimState)
                    continue;
                map<State *, string> tempTransMap;
                for (auto tState : set.second) {
                    if (tState.first == elimState) {
                        for (auto elimStateTrans : transitionMap[elimState]) {
                            if (elimStateTrans.first == elimState)
                                continue;
                            string reTrans = '(' + tState.second + ')';
                            if (selfTrans)
                                reTrans += '(' + selfTransStr + ")*";
                            reTrans += '(' + elimStateTrans.second + ')';
                            if (elimStateTrans.first == set.first) {
                                reTrans.insert(reTrans.begin(), '(');
                                reTrans += ")";
                            }
                            if (tempTransMap.find(elimStateTrans.first) == tempTransMap.cend())
                                tempTransMap.insert(make_pair(elimStateTrans.first, reTrans));
                            else
                                tempTransMap[elimStateTrans.first] =
                                        '(' + tempTransMap[elimStateTrans.first] + ")+(" + reTrans + ')';
                        }
                    } else {
                        tempTransMap.insert(make_pair(tState.first, tState.second));
                    }
                }
                if (!tempTransMap.empty())
                    tempMap.insert(make_pair(set.first, tempTransMap));
            }
            transitionMap = tempMap;

            elimStates.erase(elimStates.begin() + i);
        }

        string R = transitionMap[startState][startState];
        string S = transitionMap[startState][accState];
        string U = transitionMap[accState][accState];
        string T = transitionMap[accState][startState];

        string regex;

        if ((!T.empty() && !S.empty()) || !R.empty()) {
            if (!S.empty() && (!T.empty() || !R.empty()))
                regex += '(';
            if (!R.empty() && !S.empty()) {
                if (!T.empty())
                    regex += '(';
                regex += R;
                if (!T.empty())
                    regex += ")+";
            }
            if (!S.empty() && !T.empty()) {
                regex += '(' + S + ')';
                if (!U.empty())
                    regex += '(' + U + ")*";
                regex += '(' + T + ')';
            }
            if (!S.empty() && (!T.empty() || !R.empty()))
                regex += ")*";
        }
        if (!S.empty()) {
            regex += '(' + S + ')';
        }
        if (!U.empty() && !S.empty())
            regex += '(' +  U + ")*";

        if (regexString.empty())
            regexString = regex;
        else if (!regex.empty()) {
            regexString += "+" + regex;
        }
    }

    RE re(regexString, '&');
    return re;
}

void DFA::rename() {
    for (int i = 0; i < states.size(); ++i) {
        states[i]->name = to_string(i);
    }
}

//-------------------------------------------------------------------
// Bellow is the original createTable code using an array of strings
//-------------------------------------------------------------------

/*
    vector<State*> sorted = sortedStates();

    vector<pair<string, vector<string>>> transitions;
    for (auto state : sorted) {
        vector<string> transStates;
        for (string a : alphabet) {
            char c = a[0];
            if (state->transitions.find(a[0]) != state->transitions.end()) {
                transStates.push_back(state->transitions[c][0]->name);
            }
        }
        transitions.push_back(pair<string, vector<string>>(state->name, transStates));
    }

    // Tabel opstellen
    string table[sorted.size()][sorted.size()];
    for (int i = 0; i < sorted.size(); ++i) {
        for (int j = 0; j <= i+1 && j < sorted.size(); ++j) {
            table[i][j] = "";
            // Namen van states in eerste kolom en laatste rij
            // Afhangend eerste of laatste state weglaten
            if (j == 0 && i != sorted.size()-1) {
                table[i][j] = sorted[i+1]->name;
            } else if (i == sorted.size()-1 && j != 0) {
                table[i][j] = sorted[j-1]->name;
            } else if (j > 0 && i < (sorted.size()-1)) {
                // Bij accepting states 0 toevoegen
                if (sorted[i+1]->accepting && !sorted[j-1]->accepting)
                    table[i][j] = "X";
                else if (!sorted[i+1]->accepting && sorted[j-1]->accepting)
                    table[i][j] = "X";
                // Anders niet = '-'
                else {
                    table[i][j] = "-";
                }
            }
        }
    }

    bool newCheck = true;
    int iteration = 0;

    while (newCheck) {
        newCheck = false;
        for (int i = 0; i < sorted.size()-1; i++) {
            for (int j = 1; j <= i+1 && j < sorted.size(); j++) {
                if (table[i][j] == "X") {
                    pair<string, string> p(table[i][0], table[sorted.size()-1][j]);
                    for (int k = 0; k < alphabet.size(); ++k) {
                        for (auto p1 : transitions) {
                            for (auto p2 : transitions) {
                                if (p1 != p2 && ((p1.second[k] == p.first && p2.second[k] == p.second) ||
                                    (p1.second[k] == p.second && p2.second[k] == p.first))) {
                                    for (int row = 0; row < sorted.size()-1; row++) {
                                        for (int col = 1; col <= i+1 && col < sorted.size(); col++) {
                                            if (((table[row][0] == p1.first && table[sorted.size()-1][col] == p2.first) ||
                                                (table[row][0] == p2.first && table[sorted.size()-1][col] == p1.first)) &&
                                                table[row][col] == "-") {
                                                table[row][col] = "X";
                                                newCheck = true;
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        iteration += 1;
    }
*/
