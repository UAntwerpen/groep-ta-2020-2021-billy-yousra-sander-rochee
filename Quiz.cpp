//
// Created by Billy on 13/05/2021.
//

#include "Quiz.h"
#include "Vraag.h"
#include "algemeen.h"


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
        vragen.push_back(v);
    }

    //Close the file!
    input.close();

    initialiseVragen();
}

void Quiz::initialiseVragen() {
    for (auto vraag : vragen) {
        vraag->setupAntwoorden();
    }
}

void Quiz::selectGame() {
    //kies gamemode
    string mode;

    clock_t randomTime = clock();

    cout << "Geef de mode die je wilt spelen op:\n";
    getline(cin,mode);
    //mode = "classic 3";

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
    vector<string> input = split(mode);

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

    int r = roundToInt(((double)(clock() - randomTime)/CLOCKS_PER_SEC)*100);
    if(m == "classic") {
        return this->classicMode(nr, r);
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

void Quiz::classicMode(const int aantal, unsigned int randomTime) {
    cout << "classic mode: enabled" << endl;

    //maak een set aan die indexwaarden voor de vragenvector bijhoudt.
    //later kunnen we dan een willekeurige indexwaarde genereren en daarmee een vraag opvragen
    //aangezien het een set is, kunnen we ook eenvoudig deze waarde verwijderen zodat je in één quiz nooit tweemaal dezelfde vraag krijgt

    vector<int> r;
    for(int i = 0; i < this->vragen.size(); i++) {
        r.push_back(i);
    }

    //het spelen zelf:
    for(int n = 0; n < aantal; n++) {

        //index waarde voor 1 vraag
        int index = randomTime % r.size();
//        cout << "element to remove: " << r[index] << endl << "before removal: ";
//        for(auto in : r) {
//            cout << in << " ";
//        }

        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
         * Volgende code is een voorbeeld van hoe een antwoord nagekeken kan worden
         */
        string input;
        Vraag* vraag = vragen[r[index]];
        cout << vraag->vraag << endl;
        getline(cin, input);
        input = toLowerCase(input);

        /*
         Vector bevat alle vectoren van antwoorden die niet aanwezig waren
         Er wordt niet gegeven welk van de gegeven antwoorden correct waren

         Stel dat ik 3 antwoorden geef waar er 2 correct van waren, dan weet ik niet dewelke
         Wel welke antwoorden ik niet gegeven heb
        */
        vector<vector<string>> missed = vraag->checkAntwoord(input);

        cout << "Ontbrekende termen:\n";
        for (auto& v : missed) {
            for (auto& s : v) {
                cout << s << '\t';
            }
            cout << endl;
        }
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
         * Einde voorbeeld
         */


//        cout << endl << "after removal: ";
        r.erase(r.begin()+index);
//        for(auto in : r) {
//            cout << in << " ";
//        }
//        cout << endl;
        cout << "-----" << endl;
    }
}