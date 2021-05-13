#include <iostream>

#include "json.hpp"
#include "Vraag.h"
#include "Quiz.h"

using namespace std;


int main() {
    Quiz q("voorbeeldJson.json");

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
