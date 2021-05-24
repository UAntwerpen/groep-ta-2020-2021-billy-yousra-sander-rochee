#include <iostream>

#include "json.hpp"
#include "Vraag.h"
#include "Quiz.h"

using namespace std;


int main() {
    Quiz q("voorbeeldJson.json");
    cout << "Kies welke gamemode je wilt spelen. Doe dit door de juiste string op te geven" << endl;
    cout << "de verwachtte input is 'Mode'+'aantal vragen':" << endl << "'Classic 5' bijvoorbeeld zal een spel in classic mode starten met 5 vragen" << endl;
    cout << "gekende modi zijn: classic, blind, hardcore, killer (niet hoofdlettergevoelig)" << endl << endl;
    q.selectGame();

    return 0;
}

/*
    //niet opgeslagen als string
    char antwoord[512];

    cout << "Geef je antwoord op: ";
    cin.getline(antwoord,512);

    cout << "je gegeven antwoord was: " << antwoord << endl;



    //wel opgeslagen als string
    string antw;

    cout << "Geef je antwoord op: ";
    getline(cin,antw);

    cout << "je gegeven antwoord was: " << antw;
 */
