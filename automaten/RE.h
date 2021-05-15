//
// Created by Sander on 28/03/2021.
//

#ifndef TA_PROGRAMMEEROPDRACHT_RE_H
#define TA_PROGRAMMEEROPDRACHT_RE_H

#include <set>
#include <string>

#include "ENFA.h"
using namespace std;

/**
 * Class representing a node in a regular expression
 */
class reNode {
public:
    virtual ~reNode() {};
    virtual bool accepts(string& iStr) const = 0;
    virtual vector<State*> toENFA(vector<State*>& states) const = 0;
    virtual void print() const = 0;
};

/**
 * Class representing a regular expression
 * Consists of different nodes representing different regex operations
 */
class RE {
protected:
    char epsilon;
    set<char> alphabet;
    reNode* re = nullptr;
public:
    // Constructor and destructor
    RE(string regex, char eps);
    ~RE();
    // Checks if string is recognized by the RE
    virtual bool accepts(string iStr) const;
    // Converts RE to ENFA
    ENFA toENFA() const;

    void print() const;
};

/**
 * Class derived from reNode representing a node for singe character
 */
class charNode : public reNode {
    // Set epsilon character
    char epsilon;
    // Character represented by the node
    char c;
public:
    // Constructor
    charNode(string& iStr, char eps);
    // Checks whether string is accepted by the node
    bool accepts(string& iStr) const;
    // Converts node to ENFA states
    // Returns {start state, end state}
    vector<State*> toENFA(vector<State*>& states) const;

    void print() const;
};

/**
 * Class derived from reNode representing a node for concatenation operation
 */
class concNode : public reNode {
    char epsilon;
    // Left and right regular expressions
    reNode* lRE = nullptr;
    reNode* rRE = nullptr;
public:
    // Constructor and destructor
    concNode(string& lStr, string& rStr, char eps);
    ~concNode();
    // Checks whether string is accepted by node
    bool accepts(string& iStr) const;
    // Converts node to ENFA states
    // Returns {start state, end state}
    vector<State*> toENFA(vector<State*>& states) const;

    void print() const;
};

/**
 * Class derived from reNode representing a node for union operation
 */
class unionNode : public reNode {
    char epsilon;
    // Left and right regular expressions
    reNode* lRE = nullptr;
    reNode* rRE = nullptr;
public:
    // Constructor and destructor
    unionNode(string& lStr, string& rStr, char eps);
    ~unionNode();
    // Checks whether string is accepted by node
    bool accepts(string& iStr) const;
    // Converts node to ENFA states
    // Returns {start state, end state}
    vector<State*> toENFA(vector<State*>& states) const;

    void print() const;
};

/**
 * Class derived from reNode representing a node for kleene star operation
 */
class kleeneNode : public reNode {
    char epsilon;
    // Regular expression in kleene star
    reNode* re = nullptr;
public:
    // Constructor and destructor
    kleeneNode(string& iStr, char eps);
    ~kleeneNode();
    // Checks whether string is accepted by node
    bool accepts(string& iStr) const;
    // Converts node to ENFA states
    // Returns {start state, end state}
    vector<State*> toENFA(vector<State*>& states) const;

    void print() const;
};

#endif //TA_PROGRAMMEEROPDRACHT_RE_H
