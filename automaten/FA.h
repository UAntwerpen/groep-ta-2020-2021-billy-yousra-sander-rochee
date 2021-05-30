//
// Created by sande on 10/03/2021.
//

#ifndef TA_PROGRAMMEEROPDRACHT_FA_H
#define TA_PROGRAMMEEROPDRACHT_FA_H

#include <string>
#include <vector>
#include <map>
#include <iostream>

#include "../json.hpp"

using namespace std;
using json = nlohmann::json;

//---------------
// State Structs
//---------------

// Struct representing a state in an FA
// Struct supports ENFA, NFA and DFA
struct State{
    // Copy constructor
    State(const State& state);
    // Default constructor
    State() {}
    State(string n, bool acc, bool start) : name(n), accepting(acc), starting(start) {}

    State& operator= (const State& copy);

    string name;
    bool accepting = false;
    bool starting = false;
    // Stores the transitions from the state
    // [ transChar: { transStates }, ... ]
    map<char, vector<State*>> transitions;
};

// Struct for dead state
struct deadState : public State {
    deadState(vector<char>& alphabet);
    bool used = false;
};

//-----------------------
// Finite Automata class
//-----------------------

class FA {
protected:
    vector<State*> states;
    vector<State*> acceptingStates;
    vector<char> alphabet;
    State* startState;
    string type;
public:
    FA() { type = "FA"; startState = nullptr; }
    FA(const string &fileName);
    ~FA();

    void clear();

    // Set the alphabet of the FA
    void setAlphabet(const vector<char>& v) {alphabet = v;}

    // Add a single state to the FA
    void addState(State* state) {states.push_back(state);}
    // Set the states of the FA to given states
    void setStates(vector<State*>& v) {states = v;}

    // Add a single accepting state to the FA
    void addAccepting(State* state) {acceptingStates.push_back(state);}
    // Set the accpeting states to the given states
    void setAccepting(vector<State*>& v) {acceptingStates = v;}

    // Set the start state of the FA
    void setStart(State* state) {startState = state;}

    /**
     * Checks whether the given string gets accepted by the FA
     * @param inString: Input string
     * @return          Bool whether accepted
     */
    virtual bool accepts(const string &inString) const = 0;

    /**
     * Prints the FA in json format
     */
    virtual void print() const;

protected:
    // Create a nlohmann::json object of the FA
    virtual json createJson() const;
};


#endif //TA_PROGRAMMEEROPDRACHT_FA_H
