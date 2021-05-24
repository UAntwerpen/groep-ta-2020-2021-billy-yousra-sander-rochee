//
// Created by Billy on 13/05/2021.
//

#ifndef TA_GROEP_QUIZ_H
#define TA_GROEP_QUIZ_H

#include "json.hpp"
#include "ctime"

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <string>
#include <iterator>
#include <set>
#include <cstring>
#include <sstream>
#include <random>
//#include <algorithm>

//namespaces
using namespace std;
using json = nlohmann::json;

//forward declaration
class Vraag;

class Quiz {
public:
    //vector met pointers naar vragen
    vector<Vraag*> vragen;

    /**
     * krijgt json bestand mee om vragen aan te maken
     * Initialiseerd alle velden van vraag behalve RE
     * @param filename: name of input file
     */
    Quiz(string filename);

    /**
     * deze functie start in het begin
     * Van hieruit kan je de mode kiezen die je wil spelen
     */
    void selectGame();

    /**
     * aanroep zal een spel in classic mode starten
     * bij classic mode krijg je een aantal vragen die je moet beantwoorden
     * na elke vraag krijg je het juiste antwoord op het scherm
     * Een totale score wordt op het einde gegeven en ook een mogelijkheid om alle juiste antwoorden op te vragen
     */
    void classicMode(int aantal, unsigned int randomTime);
};


#endif //TA_GROEP_QUIZ_H
