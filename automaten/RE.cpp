//
// Created by Sander on 28/03/2021.
//

#include <stack>

#include "RE.h"

/**
 * Creates the nodes for a given regex
 * @param regexStr: Regex for which to create the nodes
 * @param eps:      Chosen epsilon character
 * @return          Pointer to starting node
 */
reNode* reNode::createReNode(string regexStr, char eps, set<char>& alpha) {
    reNode* re = nullptr;

    // Bool representing whether there is a union outside of the brackets
    // This is important for the order of the operations (skip concatenation when there is a union)
    bool Union = false;

    // Stack to keep track of open brackets
    stack<int> brackets;

    // Bool whether outside brackets should be removed
    // Default to true so that at least one loop is done
    // During loops check for union
    bool remove = true;
    while (remove) { // Remove brackets loop
        for (int i = 0; i < regexStr.size(); ++i) {
            if (regexStr[i] == '(') { // Add open bracket to stack
                brackets.push(1);
                continue;
            }
            if (regexStr[i] == ')') // Remove bracket from stack on bracket close
                brackets.pop();
            // Check whether stack gets empty before string end
            // Outside brackets must not be removed
            if ((brackets.empty() && i != regexStr.size() - 1) || regexStr.size() == 1)
                remove = false;
            // Check whether union operation with empty brackets stack
            if (brackets.empty() && regexStr[i] == '+')
                Union = true;
        }
        // Check whether outside brackets must be removed
        if (remove && regexStr[0] == '(' && regexStr[regexStr.size() - 1] == ')')
            // Delete outside brackets
            regexStr = regexStr.substr(1, regexStr.size() - 2);
        while (!brackets.empty()) // Make sure stack is empty
            brackets.pop();
    }

    // Go over regex
    for (int i = 0; i < regexStr.size(); ++i) {
        // Check brackets
        if (regexStr[i] == '(') {
            brackets.push(1);
            continue;
        } if (regexStr[i] == ')')
            brackets.pop();
        // No open brackets
        if (brackets.empty()) {
            // Only check if not last character
            if (i < regexStr.size() - 1) {
                // Union
                if (regexStr[i + 1] == '+') {
                    string lStr = regexStr.substr(0, i + 1);
                    string rStr = regexStr.substr(i + 2, regexStr.size());
                    re = new unionNode(lStr, rStr, eps, alpha);
                    break;
                } // Kleene star not at end of string, skip to next character
                else if (regexStr[i + 1] == '*' && i != regexStr.size()-2)
                    continue;
                // Kleene star at end of string
                else if (regexStr[i + 1] == '*' && i == regexStr.size()-2) {
                    string kleen = regexStr.substr(0, i+1);
                    re = new kleeneNode(kleen, eps, alpha);
                    break;
                } // Concatenation when no union
                else if (!Union) {
                    string lStr = regexStr.substr(0, i + 1);
                    string rStr = regexStr.substr(i + 1, regexStr.size());
                    re = new concNode(lStr, rStr, eps, alpha);
                    break;
                }
            }
        }
    } // Nothing yet, thus single character
    if (re == nullptr && regexStr.size() == 1)
        re = new charNode(regexStr, eps, alpha);

    return re;
}

//--------------------------//
// Regular Expression Class //
//--------------------------//
/**
 * Constructor for the RE class
 * @param regex:    Regex to be represented by the RE
 * @param eps:      Chosen epsilon character
 */
RE::RE(string regex, char eps) {
    epsilon = eps;
    // Set up alphabet
    for (char c : regex) {
        // Add normal character to alphabet
        if (c != '(' && c != ')' && c != eps && c != '+' && c != '*')
            alphabet.insert(c);
    }
    re = reNode::createReNode(regex, eps, alphabet);
}

/**
 * Creates a regex to recognise a word in a text
 * @param regex
 */
RE::RE(const string& regex) {
    textAlphabet();
    string loopStr = alphabetLoop();
    string reStr = loopStr + "(" + regex + ")" + loopStr;
    re = reNode::createReNode(reStr, epsilon, alphabet);
}

RE::~RE() {
    delete re;
}

/**
 * Checks whether a given string is accepted by the RE
 * @param iStr: String to be checked
 * @return      Bool whether accepted
 */
bool RE::accepts(string iStr) const {
    for (char c : iStr) {
        // Check for character not in alphabet or epsilon
        if (alphabet.find(c) == alphabet.end() && c != epsilon)
            return false;
    } // Check whether accepted
    return re->accepts(iStr);
}

/**
 * Converts the RE to an ENFA
 * @return  The ENFA created from the RE
 */
ENFA RE::toENFA() const {
    ENFA enfa;
    // Set epsilon character
    enfa.setEpsilon(epsilon);
    // Create and set alphabet
    vector<char> alpha;
    for (char c : alphabet) {
        alpha.push_back(c);
    }
    enfa.setAlphabet(alpha);

    // Create states
    vector<State*> states; // Vector to add states to
    vector<State*> result = re->toENFA(states); // toENFA on reNodes

    // Set start and accepting states
    enfa.setStart(result[0]);
    // ENFA created from RE always has a single accepting state if algorithms are followed
    enfa.addAccepting(result[1]);

    // Change state names to number
    for (int i = 0; i < states.size(); ++i) {
        states[i]->name = to_string(i);
    }

    // Add all states to eNFA
    enfa.setStates(states);
    return enfa;
}

/**
 * Prints the RE as a string
 */
void RE::print() const {
    re->print(); // Call print or RE nodes
    cout << endl;
}

string RE::alphabetLoop() {
    string loop = "(";
//    for (char c : alphabet) {
//        loop.push_back(c);
//        loop += "+";
//    }
//    loop += epsilon;
    loop += "~";
    loop += ")*";

    return loop;
}

void RE::textAlphabet() {
    for (char c = 97; c <= 122; ++c) {
        alphabet.insert(c);
    }
    for (char c = '0'; c <= '9'; ++c) {
        alphabet.insert(c);
    }
    alphabet.insert(' ');
    epsilon = '#';
}

//----------------------//
// Character Node Class //
//----------------------//
/**
 * Constructor of characterNode
 * @param iStr: String containing the character
 * @param eps:  Chosen epsilon character
 */
charNode::charNode(string &iStr, char eps, set<char>& alpha) {
    epsilon = eps;
    alphabet = alpha;

    if (iStr.size() == 1)
        c = iStr[0];
    if (iStr.size() == 3 && iStr[0] == '(' && iStr[2] == ')')
        c = iStr[1];
}

/**
 * Checks whether given string is accepted by the node
 * @param iStr: String to be checked
 * @return      Bool whether accepted
 */
bool charNode::accepts(string& iStr) const {
    // Check whether string == character
    if (iStr.size() == 1) {
        if (c == fullAlphabet)
            return true;
        return iStr[0] == c;
    }
    // If empty input string
    if (iStr.size() == 0)
        // Check whether node character == epsilon character
        // If node character == epsilon empty string gets accepted
        return c == epsilon;
    return false;
}

/**
 * Convert the state to ENFA states
 * @param states:   Vector in which to store the states (given by reference)
 * @return          Vector containing begin and end state {begin state, accepting state}
 */
vector<State *> charNode::toENFA(vector<State *>& states) const {
    State* sState = new State("start", false, true);
    State* eState = new State("accept", true, false);
    // Two states with transition on character
    states.push_back(sState); states.push_back(eState);
    if (c == fullAlphabet) {
        for (char i : alphabet) {
            sState->transitions.insert(pair<char, vector<State*>>(i, vector<State*>{eState}));
        }
    } else
        sState->transitions.insert(pair<char, vector<State*>>(c, vector<State*>{eState}));
    return vector<State*>{sState, eState};
}

/**
 * Prints the node
 */
void charNode::print() const {
    string str;
    str = c;
    cout << str;
}

//--------------------------//
// Concatenation Node Class //
//--------------------------//
/**
 * Constructor for concatenationNode
 * @param lStr: Regex left of concatenation
 * @param rStr: Regex right of concatenation
 * @param eps:  Chosen epsilon character
 */
concNode::concNode(string &lStr, string &rStr, char eps, set<char>& alpha) {
    epsilon = eps;
    alphabet = alpha;
    // Create nodes for left and right regex
    lRE = createReNode(lStr, eps, alpha);
    rRE = createReNode(rStr, eps, alpha);
}

concNode::~concNode() {
    delete lRE;
    delete rRE;
}

/**
 * Checks whether given string is accepted by the node
 * @param iStr: String to be checked
 * @return      Bool whether accepted
 */
bool concNode::accepts(string &iStr) const {
    // Go over all possible ways to split the string
    for (int i = 0; i <= iStr.size(); ++i) {
        // Split string into lString an rString
        string lStr = iStr.substr(0, i);
        string rStr = iStr.substr(i, iStr.size());
        // Must be accepted by left and right RE
        if (lRE->accepts(lStr) && rRE->accepts(rStr))
            return true;
    }
    return false;
}

/**
 * Convert the state to ENFA states
 * @param states:   Vector in which to store the states (given by reference)
 * @return          Vector containing begin and end state {begin state, accepting state}
 */
vector<State *> concNode::toENFA(vector<State *>& states) const {
    vector<State*> lStates;
    vector<State*> rStates;
    // Convert left and right RE to ENFA
    vector<State*> lResults = lRE->toENFA(lStates);
    vector<State*> rResults = rRE->toENFA(rStates);
    states.insert(states.cend(), lStates.cbegin(), lStates.cend());
    states.insert(states.cend(), rStates.cbegin(), rStates.cend());
    // Remove accepting and starting from correct states
    lResults[1]->accepting = false;
    rResults[0]->starting = false;
    // Add transition from left accepting node to right starting node on epsilon
    lResults[1]->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{rResults[0]}));
    return vector<State*>{lResults[0], rResults[1]};
}

/**
 * Prints the node
 */
void concNode::print() const {
    bool lBracket = false;
    bool rBracket = false;
    if (auto* l = dynamic_cast<unionNode*>(lRE))
        lBracket = true;
    if (auto* r = dynamic_cast<unionNode*>(rRE))
        rBracket = true;
    if (lBracket)
        cout << '(';
    lRE->print();
    if (lBracket)
        cout << ")";
    if (rBracket)
        cout << '(';
    rRE->print();
    if (rBracket)
        cout << ')';
}

//------------------//
// Union Node Class //
//------------------//
/**
 * Constructor for unionNode
 * @param lStr: Regex left of union
 * @param rStr: Regex right of union
 * @param eps:  Chosen epsilon character
 */
unionNode::unionNode(string &lStr, string &rStr, char eps, set<char>& alpha) {
    epsilon = eps;
    alphabet = alpha;
    // Create nodes for left and right regex
    lRE = createReNode(lStr, eps, alpha);
    rRE = createReNode(rStr, eps, alpha);
}

unionNode::~unionNode() {
    delete lRE;
    delete rRE;
}

/**
 * Checks whether given string is accepted by the node
 * @param iStr: String to be checked
 * @return      Bool whether accepted
 */
bool unionNode::accepts(string &iStr) const {
    // String must be accepted by left or right regex
    if (lRE->accepts(iStr) || rRE->accepts(iStr))
        return true;
    return false;
}

/**
 * Convert the state to ENFA states
 * @param states:   Vector in which to store the states (given by reference)
 * @return          Vector containing begin and end state {begin state, accepting state}
 */
vector<State *> unionNode::toENFA(vector<State *>& states) const {
    // Create new start and end state
    State* sState = new State("start", false, true);
    State* eState = new State("accept", true, false);
    states.push_back(sState); states.push_back(eState); // Add the states to the vector
    // Vectors to add states of left and right regex
    vector<State*> lStates;
    vector<State*> rStates;
    // Create left and right regex states
    vector<State*> lResults = lRE->toENFA(lStates);
    vector<State*> rResults = rRE->toENFA(rStates);
    // Add states to states
    states.insert(states.cend(), lStates.cbegin(), lStates.cend());
    states.insert(states.cend(), rStates.cbegin(), rStates.cend());
    // Set starting and accepting for both regex to false
    lResults[0]->starting = false;
    rResults[0]->starting = false;
    lResults[1]->accepting = false;
    rResults[1]->accepting = false;
    // Transition from start state to start states of left and right regex start states
    sState->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{lResults[0], rResults[0]}));
    // Transitions form left and right accepting states to new accepting state
    lResults[1]->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{eState}));
    rResults[1]->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{eState}));
    // Return start and accepting state
    return vector<State*>{sState, eState};
}

/**
 * Prints the node
 */
void unionNode::print() const {
    bool lBracket = false;
    bool rBracket = false;
    if (auto* l = dynamic_cast<unionNode*>(lRE))
        lBracket = true;
    if (auto* r = dynamic_cast<unionNode*>(rRE))
        rBracket = true;
    if (lBracket)
        cout << '(';
    lRE->print();
    if (lBracket)
        cout << ')';
    cout << '+';
    if (rBracket)
        cout << '(';
    rRE->print();
    if (rBracket)
        cout << ')';
}

//------------------------//
// Kleene Star Node Class //
//------------------------//
/**
 * Constructor of kleeneNode
 * @param iStr: Regex contained in node
 * @param eps:  Chosen epsilon character
 */
kleeneNode::kleeneNode(string &iStr, char eps, set<char>& alpha) {
    epsilon = eps;
    alphabet = alpha;
    // Create node for regex
    re = createReNode(iStr, eps, alpha);
}

kleeneNode::~kleeneNode() {
    delete re;
}

/**
 * Checks whether given string is accepted by the node
 * @param iStr: String to be checked
 * @return      Bool whether accepted
 */
bool kleeneNode::accepts(string &iStr) const {
    string epsStr; epsStr = epsilon;
    // Empty string is accepted by kleene star
    if (iStr == epsStr || iStr.empty())
        return true;
    bool accepts = false;
    // Itterate through string checking for math with internal RE
    // If match delete part of string and repeat until the end
    for (int i = 0; i < iStr.size()+1; ++i) {
        string lStr = iStr.substr(0, i);
        string rStr = iStr.substr(i, iStr.size());
        accepts = re->accepts(lStr) && this->accepts(rStr);
        if (accepts)
            return accepts;
    }
    return accepts;
}

/**
 * Convert the state to ENFA states
 * @param states:   Vector in which to store the states (given by reference)
 * @return          Vector containing begin and end state {begin state, accepting state}
 */
vector<State *> kleeneNode::toENFA(vector<State *> &states) const {
    State* sState = new State("start", false, true);
    State* eState = new State("accept", true, false);
    states.push_back(sState); states.push_back(eState);
    vector<State*> reStates;
    vector<State*> reResults = re->toENFA(reStates);
    states.insert(states.cend(), reStates.cbegin(), reStates.cend());
    reResults[0]->starting = false;
    reResults[1]->accepting = false;
    sState->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{eState, reResults[0]}));
    reResults[1]->transitions.insert(pair<char, vector<State*>>(epsilon, vector<State*>{eState, reResults[0]}));
    return vector<State*>{sState, eState};
}

/**
 * Prints the node
 */
void kleeneNode::print() const {
    cout << "(";
    re->print();
    cout << ")*";
}