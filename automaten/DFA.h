//
// Created by sande on 13/02/2021.
//

#ifndef TA_PROGRAMMEEROPDRACHT_DFA_H
#define TA_PROGRAMMEEROPDRACHT_DFA_H

#include <string>
#include <set>

#include "FA.h"
#include "RE.h"

using namespace std;

//class RE;

/**
 * Diterministic Finite Automaton class
 */
class DFA : public FA {
private:
    // Creates the product states starting from the two given states
    string createProductStates(map<string, State*> &stateMap, State* st1, State* st2, bool doorsnede);

    // For table filling algorithm
    // Sorts the states in the given vector
    vector<State*> sortStates(const vector<State*>& states) const;
    // Creates the transition table of the given states
    vector<pair<string, vector<string>>> createTransTable(const vector<State*>& states) const;
    // Setup for the TFA table
    vector<vector<string>> initialiseTable(const vector<State*>& states) const;
    // Run TFA
    vector<vector<string>> createTable(const vector<State*>& states) const;
    // Get all equivalent states { {eqv states 1}, {eqv states 2}, ... }
    vector<set<string>> getEqvStates() const;
    // Creates a minimized state from the state names
    void createMinimizedState(set<string>& stateNames);

public:
    // Constructors
    DFA() : FA() { type = "DFA"; };
    DFA(const string &fileName);
    // Product DFA constructor
    DFA(DFA& dfa1, DFA& dfa2, bool doorsnede);
    // Copy constructor
    DFA(const DFA& dfa);

    // Checks whether string is accepted
    bool accepts(const string &inString) const;

    // Returns a minimized version of the DFA
    DFA minimize();
    // Prints the table of the TFA
    void printTable() const;
    // Overload compare to check for equivalent DFA's
    bool operator== (DFA& check);

    RE toRE();
};


#endif //TA_PROGRAMMEEROPDRACHT_DFA_H
