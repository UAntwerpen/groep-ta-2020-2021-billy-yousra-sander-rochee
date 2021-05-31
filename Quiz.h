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

enum modeTypes{classicMode, blindMode, hardcoreMode, killerMode};

class Quiz {
    pair<int, int> printFinalResults(vector<pair<int, int>> &totaleScore, bool killer);

    void printAllResults();
    void printBlindAnswers(vector<vector<string>> &correction);

    vector<vector<string>> stelVraag(int vIndex, pair<int, int>& score);

    bool useProduct = false;
public:
    //vector met pointers naar vragen
    vector<Vraag*> vragen;

    //vector met resultaten die worden afgeprint bij het beeinigen van het spel
    vector<pair<int,int>> resultaten;
    vector<string> modes;

    /**
     * krijgt json bestand mee om vragen aan te maken
     * Initialiseerd alle velden van vraag behalve RE
     * @param filename: name of input file
     */
    Quiz(string filename);

    ~Quiz();

    void clear();

    void addFile(string filename);

    /**
     * Functie die het spel zal starten
     */
    void startGame();

    /**
     * deze functie start in het begin
     * Van hieruit kan je de mode kiezen die je wil spelen
     */
    bool selectGame(bool stop = false);

    /**
     * aanroep zal een spel in classic mode starten
     * bij classic mode krijg je een aantal vragen die je moet beantwoorden
     * na elke vraag krijg je het juiste antwoord op het scherm
     * Een totale score wordt op het einde gegeven en ook een mogelijkheid om alle juiste antwoorden op te vragen
     *
     * Keuze voor hardcore mode waarbij iedere verwachte term in het antwoord moet zitten om de vraag juist te hebben
     */
    void classicOutput(int aantal, unsigned int randomTime);

    /**
     * aanroep zal een spel in blind mode starten
     * bij blind mode krijg je een aantal vragen die je moet beantwoorden
     * na elke vraag krijg je het juiste antwoord niet op het scherm (bij classic wel)
     * een score wordt ook niet gegeven en enkel op het einde weet je je uiteindelijke score
     * op het einde kan je de juiste vragen opvragen door een commando te typen
     * het commando print alle vragen met hun juiste antwoorden uit en dan de antwoorden gegeven door de speler
     * het laatste is optioneel
     */
    void blindOutput(int aantal, unsigned int randomTime, modeTypes mode, bool product = false);
};


#endif //TA_GROEP_QUIZ_H
