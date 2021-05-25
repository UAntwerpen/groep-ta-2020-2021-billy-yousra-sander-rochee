//
// Created by sande on 25/05/2021.
//

#include "algemeen.h"

#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

//string split
vector<string> split(const string& str, char space) {
    stringstream stream(str);
    vector<string> vec;
    for(string temp; getline(stream,temp, space);)
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
        if(c >= '0' && c <= '9') {
            continue;
        } else {
            return false;
        }
    }
    return true;
}