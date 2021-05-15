//
// Created by sande on 10/03/2021.
//

#ifndef TA_PROGRAMMEEROPDRACHT_NFA_H
#define TA_PROGRAMMEEROPDRACHT_NFA_H

#include "FA.h"
#include "DFA.h"

class DFA;

class NFA: public FA {
    // Creates combined state from states given in states vector
    // Recursively creates transition states
    string createStates(map<string, State*>& stateMap, vector<State*>& states, deadState* dead);
public:
    // Constructor
    NFA(const string &fileName);

    // Checks whether string is accepted by NFA
    bool accepts(const string &inString) const;
    // Returns DFA created from the NFA
    DFA toDFA();
};


#endif //TA_PROGRAMMEEROPDRACHT_NFA_H
