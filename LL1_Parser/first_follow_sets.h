#ifndef FIRST_FOLLOW_SETS_H
#define FIRST_FOLLOW_SETS_H

#include <fstream>
#include <sstream>

#include<vector>
#include<string>
#include <unordered_map>
#include <unordered_set>

using namespace std;

inline unordered_map<string, vector<vector<string>>> readGrammar(string filename) {
    unordered_map<string, vector<vector<string>>> contextfree_grammar;
    
    ifstream file(filename);
    string line;

    while (getline(file, line)) {
        string lhs, arrow, rhs;
        
        // Read Non-Terminal (Key), "->", and productions.
        stringstream ss(line);
        ss >> lhs >> arrow; 

        vector<vector<string>> productions;
        string production;
        while (getline(ss, production, '|')) {  
            vector<string> rule;    string symbol;
            stringstream prodStream(production);
            while (prodStream >> symbol) {      rule.push_back(symbol);     }
            
            productions.push_back(rule);
        }
        
        // Store the Non-terminal and all its productions as key-value pair.
        contextfree_grammar[lhs] = productions;  
    }

    return contextfree_grammar;
}

inline unordered_set<char> computeFirst(unordered_map<string, unordered_set<char>> &FIRST, string symbol, unordered_map<string, vector<vector<string>>> contextfree_grammar) {
    if (!FIRST[symbol].empty()) return FIRST[symbol]; 

    unordered_set<char> firstSet;
    // If the first symbol is a non-terminal or some empty grammar, return it.
    if (islower(symbol[0]) || symbol == "~") { 
        firstSet.insert(symbol[0]);
        return firstSet;
    }

    for (auto& production : contextfree_grammar[symbol]) {
        for (auto& symbol : production) {
            unordered_set<char> subFirst = computeFirst(FIRST, symbol, contextfree_grammar);
            firstSet.insert(subFirst.begin(), subFirst.end());
            if (subFirst.find('~') == subFirst.end())   break;
        }
    }

    FIRST[symbol] = firstSet;
    return firstSet;
}

inline void addFirstToFollow(unordered_map<string, unordered_set<char>>& FOLLOW, const string& B, const unordered_set<char>& firstNext) {
    for (char c : firstNext) {
        if (c != '~') FOLLOW[B].insert(c);
    }
}

inline void computeFollow(unordered_map<string, unordered_set<char>>& FOLLOW, const unordered_map<string, unordered_set<char>>& FIRST, const string& startSymbol, unordered_map<string, vector<vector<string>>>& contextfree_grammar) {
    FOLLOW[startSymbol].insert('$'); // Start symbol gets $

    bool updated = true;
    while (updated) {
        updated = false;

        for (auto it = contextfree_grammar.begin(); it != contextfree_grammar.end(); ++it) {
            const string& lhs = it->first;
            const auto& rules = it->second;

            for (const auto& rule : rules) {
                for (size_t index = 0; index < rule.size(); index++) {
                    const string& B = rule[index];
                    if (!isupper(B[0])) continue; // Skip terminals

                    size_t prevSize = FOLLOW[B].size();

                    // If there's a next symbol, add its FIRST set.
                    if (index + 1 < rule.size()) {
                        const string& nextSymbol = rule[index + 1];
                        const auto& firstNext = FIRST.at(nextSymbol); // Use precomputed FIRST

                        addFirstToFollow(FOLLOW, B, firstNext);

                        // If epsilon (~) is in FIRST(nextSymbol), inherit FOLLOW(lhs)
                        if (firstNext.count('~')) {
                            FOLLOW[B].insert(FOLLOW[lhs].begin(), FOLLOW[lhs].end());
                        }
                    }
                    // If B is the last symbol, inherit FOLLOW(lhs)
                    else {
                        FOLLOW[B].insert(FOLLOW[lhs].begin(), FOLLOW[lhs].end());
                    }

                    // Check if FOLLOW[B] changed
                    if (FOLLOW[B].size() > prevSize) updated = true;
                }
            }
        }
    }
}


#endif