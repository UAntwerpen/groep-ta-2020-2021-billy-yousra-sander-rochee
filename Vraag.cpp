//
// Created by Billy on 12/05/2021.
//

#include "Vraag.h"
#include "algemeen.h"

#include <ctime>

void Vraag::setupAntwoorden() {
    if (!antwoordDFAs.empty())
        return;
    for (auto& set : antwoorden) {
        string antwoordStr;

        for (auto& antwoord : set) {
            antwoordStr += antwoord + "+";
        }
        antwoordStr.replace(antwoordStr.length()-1, 1, "");
        antwoordStr = toLowerCase(antwoordStr);

//        cout << antwoordStr << endl;

//        clock_t startT = clock();

        RE re(antwoordStr);

//        printf("Setup RE: %.2fs\t", (double)(clock() - startT) / CLOCKS_PER_SEC);
//        startT = clock();

        ENFA enfa = re.toENFA();

//        printf("Setup ENFA: %.2fs\t", (double)(clock() - startT) / CLOCKS_PER_SEC);
        clock_t startT = clock();

        DFA dfa = enfa.toDFA().minimize();

        printf("Setup DFA: %.2fs\t", (double)(clock() - startT) / CLOCKS_PER_SEC);
//        startT = clock();

        antwoordDFAs.push_back(dfa);

//        printf("Minimize DFA: %.2fs\n", (double)(clock() - startT) / CLOCKS_PER_SEC);
    }
}

void removeUnknown(string& input) {
    for (char& c : input) {
        if (!(c >= 'a' && c <= 'z') && c != ' ' && !(c >= '0' && c <= '9'))
            c = ' ';
    }
}

vector<vector<string>> Vraag::checkAntwoord(string &input, pair<int,int> &score) const {
    // Verwijder alle nietgekende tekens uit een antwoord
    // Enkel Spaties en alphanumerieke waarden worden nagekeken
    removeUnknown(input);

    vector<vector<string>> ontbrekendeAntwoorden;
    // Stel aantal punten voor huidige vraag op
    score.second = antwoordDFAs.size();
    // Ga over de DFA's voor de verschillende verwachte woorden
    for (int i = 0; i < antwoordDFAs.size(); ++i) {
        // Kijk of woord voorkomt
        if (!antwoordDFAs[i].accepts(input))
            ontbrekendeAntwoorden.push_back(antwoorden[i]);
        else
            score.first++;
    }
    return ontbrekendeAntwoorden;
}