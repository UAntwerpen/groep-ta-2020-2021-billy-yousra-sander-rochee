//
// Created by Billy on 12/05/2021.
//

#include "Vraag.h"
#include "algemeen.h"

#include <ctime>

void Vraag::setupAntwoorden() {
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
//        startT = clock();

        DFA dfa = enfa.toDFA();

//        printf("Setup DFA: %.2fs\t", (double)(clock() - startT) / CLOCKS_PER_SEC);
//        startT = clock();

        antwoordDFAs.push_back(dfa);

//        printf("Minimize DFA: %.2fs\n", (double)(clock() - startT) / CLOCKS_PER_SEC);
    }
}

vector<vector<string>> Vraag::checkAntwoord(string &input, pair<int,int> &score) const {
    vector<vector<string>> ontbrekendeAntwoorden;
    score.second = antwoordDFAs.size();
    for (int i = 0; i < antwoordDFAs.size(); ++i) {
        if (!antwoordDFAs[i].accepts(input))
            ontbrekendeAntwoorden.push_back(antwoorden[i]);
        else
            score.first++;
    }
    return ontbrekendeAntwoorden;
}
