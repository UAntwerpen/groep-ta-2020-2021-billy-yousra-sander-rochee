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

pair<int, int> Quiz::printFinalResults(vector<pair<int, int>> &totaleScore, bool killer) {
    int correct = 0, totaal = 0;
    for(pair<int,int> &punt : totaleScore) {
        correct += punt.first;
        totaal += punt.second;
    }

    cout << endl << "+--+===+-----------------------------------";
    if(correct >= 10) { cout << "--";} else { cout << "-"; }
    if(totaal >= 10) { cout << "--";} else { cout << "-"; }
    cout << "+===+--+\n| Je behaalde een totale score van " << correct << '/' << totaal;
    if( ((double)correct/totaal >= 0.5 && !killer) || ((double)correct/totaal == 1 && killer) ) {
        cout << " --> Gewonnen |";
    } else {
        cout << " --> Verloren |";
    }
    cout << endl << "+--+===+-----------------------------------";
    if(correct >= 10) { cout << "--";} else { cout << "-"; }
    if(totaal >= 10) { cout << "--";} else { cout << "-"; }
    cout << "+===+--+\n\n";
    pair<int,int> ret_val;
    ret_val.first = correct;
    ret_val.second = totaal;
    return ret_val;
}

void Quiz::printAllResults() {
    cout << "+=+---------------------+=+" << endl; //27

    for(int n = 0; n < resultaten.size(); n++) {
        string out = "| Spel " + to_string(n+1) + " (" + modes[n] + "): " + to_string(resultaten[n].first) + "/" + to_string(resultaten[n].second);
        int extra = 27-out.size()-1;
        for(int i = 0; i < extra; i++) {
            out += ' ';
        }
        cout << out << '|' << endl;

    }

    cout << "+=+---------------------+=+" << endl; //27
}

void Quiz::printBlindAnswers(vector<vector<string>> &correction) {
    int nr = 1;
    for(auto &str_vec : correction) {
        cout << "vraag " << nr << ": " << str_vec[0] << endl;
        cout << "verwacht: " << str_vec[1] << endl;
        cout << "verkregen: " << str_vec[2] << endl << endl;
        nr++;
    }
}

void Quiz::startGame() {
    //uitleg
    cout << "Kies welke gamemode je wilt spelen. Doe dit door de juiste string op te geven" << endl;
    cout << "de verwachtte input is 'Mode'+'aantal vragen':" << endl << "'Classic 5' bijvoorbeeld zal een spel in classic mode starten met 5 vragen" << endl;
    cout << "gekende modi zijn: classic, blind, hardcore, killer (niet hoofdlettergevoelig)" << endl << endl;

    this->selectGame();
}

void Quiz::selectGame(bool stop) {
    //kies gamemode
    string mode;

    clock_t randomTime = clock();
    if(stop) {
        cout << "als je het spel wilt beeindigen, type dan het woordje 'stop', Anders:\n";
    }
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

    if(m=="stop" && stop) {
        //eventueel hier iets doen met de score
        return this->printAllResults();
    }

    if (input.size() < 2) {
        cout << "Geen correcte string voor de modus opgegeven!" << endl;
        return selectGame();
    }

    //sla het nummer van de tweede op als int
    bool check = checkInt(input[1]);
    int nr;
    if(check) {
        nr = stoi(input[1]);
    } else {
        cout << "geef een geldig nummer op" << endl << endl;
        return selectGame();
    }
    if(nr == 0) {
        cout << "geef minstens 1 op als aantal" << endl << endl;
        return selectGame();
    }

    int r = roundToInt(((double)(clock() - randomTime)/CLOCKS_PER_SEC)*100);
    if(m == "classic") {
        return this->classicOutput(nr, r, false);
    } else if(m == "blind") {
        return this->blindOutput(nr, r, false);
    } else if(m == "hardcore") {
        return this->classicOutput(nr, r, true);
    } else if (m == "killer") {
        return this->blindOutput(nr, r, true);
    } else {
        cout << "de ingegeven modus wordt niet herkend" << endl << endl;
        return selectGame();
    }
}

vector<vector<string>> Quiz::stelVraag(int vIndex, pair<int, int> &score) {
    string input;
    Vraag* vraag = vragen[vIndex];
    cout << vraag->vraag << endl;
    getline(cin, input);
    input = toLowerCase(input);

    return vraag->checkAntwoord(input, score);
}

void Quiz::classicOutput(int aantal, unsigned int randomTime, bool hardcore) {
    cout << "classic mode: enabled" << endl;

    //maak een set aan die indexwaarden voor de vragenvector bijhoudt.
    //later kunnen we dan een willekeurige indexwaarde genereren en daarmee een vraag opvragen
    //aangezien het een set is, kunnen we ook eenvoudig deze waarde verwijderen zodat je in één quiz nooit tweemaal dezelfde vraag krijgt

    vector<int> r;
    for(int i = 0; i < this->vragen.size(); i++) {
        r.push_back(i);
    }

    vector<pair<int,int>> totaleScore;

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
//        string input;
//        Vraag* vraag = vragen[r[index]];
//        cout << vraag->vraag << endl;
//        getline(cin, input);
//        input = toLowerCase(input);

        /*
         Vector bevat alle vectoren van antwoorden die niet aanwezig waren
         Er wordt niet gegeven welk van de gegeven antwoorden correct waren

         Stel dat ik 3 antwoorden geef waar er 2 correct van waren, dan weet ik niet dewelke
         Wel welke antwoorden ik niet gegeven heb
        */

        //eerste int houdt het aantal accepterende DFA's bij en de tweede het aantal vragen
        //als je score.first == 2 en score.second == 3 hebt, dan heb je 2/3 gescoord
        pair<int,int> score;
        vector<vector<string>> missed = stelVraag(r[index], score); /*vraag->checkAntwoord(input, score);*/

        if (hardcore) {
            score.first = floor(score.first/score.second);
            score.second = score.second/score.second;
        }

        cout << "Ontbrekende antwoorden:\n";
        if(missed.empty()) {
            cout << "--> geen" << endl;
        }
        for (auto& v : missed) {
            cout << "- ";
            for (int i = 0; i < v.size(); ++i) {
                cout << v[i];
                if (i + 1 != v.size())
                    cout << ", ";
            }
            cout << endl;
        }
        /*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
         * Einde voorbeeld
         */
        cout << "score: " << score.first << '/' << score.second << endl;
        totaleScore.push_back(score);

        r.erase(r.begin()+index);
        cout << "-----" << endl;
    }

    pair<int,int> p = this->printFinalResults(totaleScore, false);
    this->resultaten.push_back(p);
    if(hardcore) {
        this->modes.emplace_back("hardcore");
    } else {
        this->modes.emplace_back("classic");
    }

    return this->selectGame(true);
}

void Quiz::blindOutput(int aantal, unsigned int randomTime, bool killer) {
    cout << "blind mode: enabled" << endl;

    //maak een set aan die indexwaarden voor de vragenvector bijhoudt.
    //later kunnen we dan een willekeurige indexwaarde genereren en daarmee een vraag opvragen
    //aangezien het een set is, kunnen we ook eenvoudig deze waarde verwijderen zodat je in één quiz nooit tweemaal dezelfde vraag krijgt

    vector<int> r;
    for(int i = 0; i < this->vragen.size(); i++) {
        r.push_back(i);
    }

    vector<pair<int,int>> totaleScore;
    vector<vector<string>> stringToPrint;

    bool wrongAnswer = false;

    //het spelen zelf:
    for(int n = 0; n < aantal; n++) {

        if (wrongAnswer) {
            totaleScore.emplace_back(0, 1);
            continue;
        }

        vector<string> blind;
        //index waarde voor 1 vraag
        int index = randomTime % r.size();

        string input;
        Vraag* vraag = vragen[r[index]];
        cout << vraag->vraag << endl;

        blind.push_back(vraag->vraag);

        string expected;
        for(auto &antwoordset : vraag->antwoorden) {
            for(auto &ant : antwoordset) {
                expected+=ant; expected+='/';
            }
            expected.pop_back();expected+="  ";
        }
        blind.push_back(expected);

        getline(cin, input);
        blind.push_back(input);
        input = toLowerCase(input);


        //eerste in houdt het aantal accepterende DFA's bij en de tweede het aantal vragen
        //als je score.first == 2 en score.second == 3 hebt, dan heb je 2/3 gescoord
        pair<int,int> score;
        vector<vector<string>> missed = vraag->checkAntwoord(input, score);

        if (killer) {
            score.first = floor(score.first / score.second);
            score.second = score.second / score.second;
            if (score.first == 0)
                wrongAnswer = true;
        }

        //cout << "score: " << score.first << '/' << score.second << endl;
        totaleScore.push_back(score);

        r.erase(r.begin()+index);
        cout << "-----" << endl;
        stringToPrint.push_back(blind);
    }

    pair<int,int> p = this->printFinalResults(totaleScore, killer);
    this->resultaten.push_back(p);
    if(killer) {
        this->modes.emplace_back("killer");
    } else {
        this->modes.emplace_back("blind");
    }

    //functie op antwoorden uit te printen
    string a;
    cout << "typ 'ja' als je je antwoorden wilt uitprinten. anders typ je wat je wil om verder te gaan" << endl;
    getline(cin, a);
    a = toLowerCase(a);
    if(a == "ja") {
        this->printBlindAnswers(stringToPrint);
    }
    cout << endl;
    return this->selectGame(true);
}