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
    // DFA's om antwoorden met na te kijken
    vector<DFA> antwoordDFAs;

    DFA product;

    /**
     * empty constructor
     */
    Vraag() {}

    /**
     * Creëert de DFA's om de antwoorden na te kijken
     */
    void setupAntwoorden();

    void setupProduct();

    /**
     * Kijkt welke verwachte woorden niet voorkomen in het gegeven antwoord
     * @param input:    Het gegecen antwoord
     * @param score:    Pair voor behaalde score en max score
     * @return      Vector met alle ontbrekende woorden
     */
    vector<vector<string>> checkAntwoord(string &input, pair<int, int> &score, bool useProduct = false) const;
};


#endif //TA_GROEP_VRAAG_H
