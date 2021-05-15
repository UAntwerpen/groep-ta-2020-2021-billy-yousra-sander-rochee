//
// Created by sande on 21/03/2021.
//

#ifndef TA_PROGRAMMEEROPDRACHT_ENFA_H
#define TA_PROGRAMMEEROPDRACHT_ENFA_H

#include <vector>

#include "FA.h"
class DFA;

class ENFA : public FA {
    // chosen epsilon character
    char epsilon;

    // Creates combined state from states in given states vector
    // Recursively creates transition states
    string createStates(map<string, State*>& stateMap, vector<State*>& states, deadState* dead);
public:
    // Constructors
    ENFA(const string &fileName);
    ENFA() : FA() { type = "ENFA"; }

    // Sets the chosen epsilon character
    void setEpsilon(char eps) { epsilon = eps; }


    void printStats() const;

    bool accepts(const string &inString) const;

    vector<State*> eClose(State* sState, vector<State*> passed = {}) const;
    DFA toDFA();
protected:
    virtual json createJson() const;
};


#endif //TA_PROGRAMMEEROPDRACHT_ENFA_H
