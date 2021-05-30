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
    for (auto& set : stateMap) {
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
    this->clear();

    alphabet = dfa.alphabet;
    type = dfa.type;

    map<string, State*> stateMap;
    // Go over states in original DFA
    for (auto state : dfa.states) {
        // Create new state
        State* nState = new State;
        // Copy parameters from state in original DFA
        *(nState) = *(state);
        states.push_back(nState); // Add new state to new DFA
        stateMap[nState->name]= nState;
    }
    // Loop over states
    for (auto fState : states) {
        // Second loop over states
        for (auto c : alphabet) {
            State* oDes = fState->transitions[c][0];
            fState->transitions[c] = {stateMap[oDes->name]};
        }
        // Add state to correct parameters if necessary
        if (fState->starting)
            startState = fState;
        if (fState->accepting)
            acceptingStates.push_back(fState);
    }
}

DFA & DFA::operator=(const DFA &check) {
    this->clear();

    alphabet = check.alphabet;
    type = check.type;

    map<string, State*> stateMap;
    // Go over states in original DFA
    for (auto state : check.states) {
        // Create new state
        State* nState = new State;
        // Copy parameters from state in original DFA
        *(nState) = *(state);
        states.push_back(nState); // Add new state to new DFA
        stateMap[nState->name]= nState;
    }
    // Loop over states
    for (auto fState : states) {
        // Second loop over states
        for (auto c : alphabet) {
            State* oDes = fState->transitions[c][0];
            fState->transitions[c] = {stateMap[oDes->name]};
        }
        // Add state to correct parameters if necessary
        if (fState->starting)
            startState = fState;
        if (fState->accepting)
            acceptingStates.push_back(fState);
    }
    return *this;
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
 * Creates a transition table for the given states
 * Table consists of maps per alphabet character linking a state with the states that transition to it
 * @param states:       Sorted states for which to make the table
 * @param transMap:     Output, Location to create map
 */
void DFA::createTransTable(const vector<State *> &states,
                           vector<map<State *, set<State *>>> &transMap /* Output */) const {
    // Go over characters in alphabet
    for (auto c : alphabet) {
        // Store the transitions to de different states
        map<State*, set<State*>> transTo;

        for (State* state : states) {
            State* desState = state->transitions[c][0];
            transTo[desState].insert(state);
        }
        transMap.push_back(transTo);
    }
}

/**
 * Setup the table for the TFA
 * @param states:   Sorted states for which to make the table
 * @param table:    Location for table
 * @return          Vector containing cords of marked fields in table
 */
vector<pair<int, int>> DFA::initialiseTable(const vector<State *> &states,
                                            vector<vector<string>> &table /* Output */) const {
    // Keep track of newly marked spots on the table
    vector<pair<int, int>> markedCords;
    // Go over rows
    for (int i = 0; i <= states.size() - 2; ++i) {
        vector<string> row;
        // Go over collums
        for (int j = 0; j <= i; ++j) {
            row.emplace_back("");

            // row_state = row+1; collum_state = collum;
            // If one of both states is accepting spot should be marked
            if ((states[i+1]->accepting && !states[j]->accepting) || (!states[i+1]->accepting && states[j]->accepting))
            {
                row[j] = "X";
                // Keep track of marked spots
                markedCords.emplace_back(i, j);
            }
            else {
                // Nothing on the spot
                row[j] = "-";
            }
        }
        // Add row to the table
        table.push_back(row);
    }

    return markedCords;
}

/**
 * Creates a completely filled TFA table
 * @param states:   Sorted states for which to create the table
 * @param table:    Location of table
 */
void DFA::createTable(const vector<State *> &states, vector<vector<string>> &table /* Output */) const {
    // Get transition table
    vector<map<State*, set<State*>>> transMap;
    createTransTable(states, transMap);
    // Get starting table
    vector<pair<int, int>> marked = initialiseTable(states, table);

    // Newly marked fields
    while (!marked.empty()) {
        // Place to temporarily store new marked spots
        vector<pair<int, int>> tempMarked;
        // Go over all marked fields in table
        for (auto& cords : marked) {
            // Get states linked to field
            State* verS = states[cords.first + 1];
            State* horS = states[cords.second];

            // Go over all transitions on the different alphabet characters
            for (auto& c : transMap) {
                // Go over transition states to first state
                for (State* s1 : c[verS]) {
                    // Go over transition states to second state
                    for (State* s2 : c[horS]) {
                        // Get index for row/collum of transition states
                        unsigned int i1 = find(states.begin(), states.end(), s1) - states.begin();
                        unsigned int i2 = find(states.begin(), states.end(), s2) - states.begin();

                        // Cords fit in the table
                        // First state of ordered states always collum cord
                        if ((i1 == 0) || (i2 >= table[i1 - 1].size()))
                            swap(i1, i2);
                        // Row index is 1 less than state index
                        i1 -= 1;
                        // Add new mark if not yet marked
                        if (table[i1][i2] != "X") {
                            table[i1][i2] = "X";
                            // Store new marked location
                            tempMarked.emplace_back(i1, i2);
                        }
                    }
                }
            }
        }
        // Update marked locations
        marked = tempMarked;
    }
}

/**
 * Gets al the equivalent states of the DFA
 * @return  Vector containing sets of equivalent states
 */
vector<set<string>> DFA::getEqvStates() const {
    // Create the table
    vector<State*> sorted = sortStates(states);
    vector<vector<string>> table;
    createTable(sorted, table);

    // Vector to store sets of equivalent states
    vector<set<string>> eqvStates;

    // Loop over rows of the table
    for (int i = 0; i < table.size(); ++i) {
        // Loop over collums of the table
        for (int j = 0; j < table[i].size(); ++j) {
            // Empty field
            if (table[i][j] == "-") {
                // Get states linked to field
                State* verS = sorted[i + 1];
                State* horS = sorted[j];

                // Check to see if states are added to existing set
                bool added = false;
                // Go over sets of equivalent states
                for (auto& set : eqvStates) {
                    // Set containing one of the states exists
                    if (set.find(verS->name) != set.cend() || set.find(horS->name) != set.cend()) {
                        // Add the states to the set
                        set.insert(verS->name);
                        set.insert(horS->name);
                        added = true;
                        break;
                    }
                }
                // States not added to existing set
                if (!added) {
                    // Create new set of states
                    eqvStates.push_back(set<string>{verS->name, horS->name});
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
    vector<State*> sorted = sortStates(states);
    vector<vector<string>> table;
    createTable(sorted, table);

    // Loop over table to print the table
    for (int i = 0; i < table.size(); ++i) {
        cout << sorted[i + 1]->name << "\t";
        for (int j = 0; j <= i+1 && j < table.size(); ++j) {
            cout << table[i][j] << '\t';
        }
        cout << endl;
    }
    cout << "\t";
    for (int i = 0; i <= sorted.size() - 2; ++i) {
        cout << sorted[i] << "\t";
    }
    cout << endl;
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
    vector<vector<string>> table;
    createTable(allStates, table);
    bool eqv = false;
    int maxRow = table.size()-1;
    // Loop over table
    for (int i = 0; i < table.size(); ++i) {
        cout << allStates[i + 1] << "\t";
        for (int j = 0; j < table[i].size(); ++j) {
            cout << table[i][j] << '\t';
            // If row and collum state are both start states
            State* verS = allStates[i + 1];
            State* horS = allStates[j];

            if ((verS == startState && horS == check.startState) ||
                (verS == check.startState && horS == startState))
                // Check whether equivalent states (empty spot)
                eqv = (table[i][j] == "-");
        }
        cout << endl;
    }

    cout << "\t";
    for (int i = 0; i <= allStates.size() - 2; ++i) {
        cout << allStates[i] << "\t";
    }
    cout << endl;

    return eqv;
}

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

bool DFA::empty() {
    return states.empty();
}
