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

using namespace std;

class Vraag {
public:
    string type;
    string vraag;
    vector<vector<string>> antwoorden;

    string RE;

    /**
     * empty constructor
     */
    Vraag() {}
};


#endif //TA_GROEP_VRAAG_H
