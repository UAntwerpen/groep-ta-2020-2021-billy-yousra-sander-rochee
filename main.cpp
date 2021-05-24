#include <iostream>

#include "json.hpp"
#include "Vraag.h"
#include "Quiz.h"

#include "automaten/RE.h"
#include "automaten/DFA.h"
#include "automaten/ENFA.h"

using namespace std;


int main() {
//    Quiz q("voorbeeldJson.json");
//
    RE re("test");
    RE regex("appeltaart");

    ENFA enfa = re.toENFA();
    DFA dfa = enfa.toDFA();
    dfa.rename();

    clock_t tStart = clock();
    DFA minDFA = dfa.minimize();
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    DFA dfa2 = regex.toENFA().toDFA();
    tStart = clock();
    DFA minDFA2 = dfa2.minimize();
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    cout << boolalpha << enfa.accepts("een test") << endl;
    cout << boolalpha << enfa.accepts("dit#is#er#ook#een") << endl;
    cout << boolalpha << enfa.accepts("this#string#is#for#testing#purposes#only") << endl;
    cout << "DFA: " << endl;
    cout << boolalpha << dfa.accepts("eentest") << endl;
    cout << boolalpha << dfa.accepts("ditiserookeen") << endl;
    cout << boolalpha << dfa.accepts("thisstringisfortestingpurposesonly") << endl;

    cout << "minDFA: " << endl;
    cout << boolalpha << minDFA.accepts("eentest") << endl;
    cout << boolalpha << minDFA.accepts("ditiserookeen") << endl;
    cout << boolalpha << minDFA.accepts("thisstringisfortestingpurposesonly") << endl;

    DFA product(minDFA, minDFA2, true);
    cout << "Product DFA: " << endl;
    cout << boolalpha << product.accepts("eentestappeltaart") << endl;
    cout << boolalpha << product.accepts("ditiserookeen") << endl;
    cout << boolalpha << product.accepts("thisstringisfortestingpurposesonly") << endl;

//    RE r = product.toRE();
//    r.print();

    tStart = clock();
    DFA minProduct = product.minimize();
    printf("Time taken: %.2fs\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);
    cout << "Min Product DFA: " << endl;
    cout << boolalpha << minProduct.accepts("eentestappeltaart") << endl;
    cout << boolalpha << minProduct.accepts("ditiserookeen") << endl;
    cout << boolalpha << minProduct.accepts("thisstringisfortestingpurposesonly") << endl;

    minProduct.print();

//    RE productRE = minProduct.toRE();
//    productRE.print();

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
