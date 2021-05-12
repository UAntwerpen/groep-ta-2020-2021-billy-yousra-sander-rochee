#include <iostream>

#include "json.hpp"

using namespace std;

int main() {
    char antwoord[512];

    cout << "Geef je antwoord op: ";
    cin.getline(antwoord,512);

    cout << "je gegeven antwoord was: " << antwoord;

    return 0;
}
