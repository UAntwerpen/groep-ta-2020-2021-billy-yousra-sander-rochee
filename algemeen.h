//
// Created by sande on 25/05/2021.
//

#ifndef TA_GROEP_ALGEMEEN_H
#define TA_GROEP_ALGEMEEN_H

#include <string>
#include <vector>
#include <cmath>

using namespace std;


//string split
vector<string> split(const string& str, char space = ' ');

//from uppercase to lowercase
string toLowerCase(string s);

bool checkInt(string &a);

inline int roundToInt(double d)
{
    return static_cast<int>(std::round(d));
}


#endif //TA_GROEP_ALGEMEEN_H
