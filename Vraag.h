//
// Created by Billy on 12/05/2021.
//

#ifndef TA_GROEP_VRAAG_H
#define TA_GROEP_VRAAG_H

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include "automaten/DFA.h"

using namespace std;

class Vraag {
public:
    string type;
    string vraag;
    vector<vector<string>> antwoorden;

    vector<DFA> antwoordDFAs;

    /**
     * empty constructor
     */
    Vraag() {}

    void setupAntwoorden();

    vector<vector<string>> checkAntwoord(string& input) const;
};


#endif //TA_GROEP_VRAAG_H
