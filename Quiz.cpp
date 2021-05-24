//
// Created by Billy on 13/05/2021.
//

#include "Quiz.h"
#include "Vraag.h"

#include <cstring>
#include <sstream>

Quiz::Quiz(string filename) {
// inlezen uit file
    std::ifstream input(filename);
    json j;
    input >> j;

    for(auto vraag : j["vragen"]) {

        //maak nieuwe state
        Vraag* v = new Vraag();

        //initialiseer de velden type en vraag
        v->type = vraag["type"];
        v->vraag = vraag["vraag"];

        //vector waarin elk antwoord wordt opgeslagen
        vector<string> antwoord;

        //ga over de antwoorden
        for(auto ant : vraag["antwoorden"]) {

            //ga over elk toegelaten antwoord (per antwoord)
            for(auto &el : ant.items()) {

                //voeg dit toe aan de vector van dat antwoord
                antwoord.push_back(el.value());
            }
            //voeg antwoord toe aan de vector van mogelijke antwoorden
            v->antwoorden.push_back(antwoord);

            //maak antwoord leeg voor volgende iteratie
            antwoord.clear();
        }
    }

    //Close the file!
    input.close();
}

//string split
vector<string> split(const string& str, char delim = ' ') {
    stringstream stream(str);
    vector<string> vec;
    for(string temp; getline(stream,temp, delim);)
        vec.push_back(temp);
    return vec;
}

//from uppercase to lowercase
string toLowerCase(string s) {
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    return s;
}

bool checkInt(string &a) {
    for(char &c : a) {
        if(c=='0'||c=='1'||c=='2'||c=='3'||c=='4'||c=='5'||c=='6'||c=='7'||c=='8'||c=='9') {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

void Quiz::selectGame() {
    //kies gamemode
    string mode;

    cout << "Geef de mode die je wilt spelen op: ";
    getline(cin,mode);

    //zorg ervoor dat eventuele spaties in het begin er worden uitgehaald
    int k = 0;
    for(char c : mode) {
        if(c == ' ') {
            k++;
        } else {
            break;
        }
    }
    mode = mode.substr(k);

    //split de string in bruikbare delen. We gebruiken enkel de eerste 2
    vector<string> input = split(mode, ' ');

    //maak de eerste lowercase
    string m = toLowerCase(input[0]);

    //sla het nummer van de tweede op als int
    bool check = checkInt(input[1]);
    int nr;
    if(check) {
        nr = stoi(input[1]);
    } else {
        cout << "geef een geldig nummer op" << endl << endl;
        return selectGame();
    }


    if(m == "classic") {
        return classicMode(nr);
    } else if(m == "blind") {
        cout << "blind bestaat nog niet" << endl;
        return;
    } else if(m == "hardcore") {
        return;
    } else {
        cout << "de ingegeven modus wordt niet herkend" << endl << endl;
        return selectGame();
    }
}

void Quiz::classicMode(const int aantal) {
    cout << "called" << endl;

}