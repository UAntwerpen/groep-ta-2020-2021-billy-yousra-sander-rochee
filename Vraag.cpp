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
//        clock_t startT = clock();

        DFA dfa = enfa.toDFA().minimize();
        dfa.rename();

//        printf("Setup DFA: %.4fs\t", (double)(clock() - startT) / CLOCKS_PER_SEC);
//        startT = clock();

        antwoordDFAs.push_back(dfa);

//        printf("Minimize DFA: %.2fs\n", (double)(clock() - startT) / CLOCKS_PER_SEC);
    }
}

void Vraag::setupProduct() {
    if (!product.empty())
        return;
    if (antwoordDFAs.empty())
        this->setupAntwoorden();

    product = antwoordDFAs[0];

    for (int i = 1; i < antwoordDFAs.size(); ++i) {
        DFA tempProduct(product, antwoordDFAs[i], true);
        product = tempProduct.minimize();
    }
}

void removeUnknown(string& input) {
    for (char& c : input) {
        if (!(c >= 'a' && c <= 'z') && c != ' ' && !(c >= '0' && c <= '9'))
            c = ' ';
    }
}

vector<vector<string>> Vraag::checkAntwoord(string &input, pair<int, int> &score, bool useProduct) const {
    // Verwijder alle nietgekende tekens uit een antwoord
    // Enkel Spaties en alphanumerieke waarden worden nagekeken
    removeUnknown(input);

    vector<vector<string>> ontbrekendeAntwoorden;

    if (useProduct) {
        score.second = 1;
        if (this->product.accepts(input))
            score.first++;
    } else {

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
    }
    return ontbrekendeAntwoorden;
}