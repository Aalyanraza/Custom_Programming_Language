#ifndef LL1PARSER_H
#define LL1PARSER_H

#include <iostream>
#include<vector>
#include <unordered_map>
#include <unordered_set>
using namespace std;


inline unordered_set<char> computeFirstOfSequence(const vector<string>& symbols, const unordered_map<string, unordered_set<char>>& FIRST) {
    unordered_set<char> result;
    for (const string& symbol : symbols) {
        if (FIRST.count(symbol)) { // Non-terminal
            const unordered_set<char>& firstOfSymbol = FIRST.at(symbol);
            result.insert(firstOfSymbol.begin(), firstOfSymbol.end());
            if (!firstOfSymbol.count('~')) {    break;  }
        }
        else { 
            result.insert(symbol[0]); 
            break;
        }
    }
    return result;
}

// Function to construct the LL(1) parsing table
inline void constructLL1Table(unordered_map<string, unordered_map<char, string>>& parsingTable, 
                        unordered_map<string, unordered_set<char>> FIRST,
                        unordered_map<string, unordered_set<char>> FOLLOW,
                        const unordered_map<string, vector<vector<string>>>& productions) {
    // Iterate over each production rule
    for (const auto& prod : productions) {
        const string& nonTerminal = prod.first; 
        const vector<vector<string>>& rules = prod.second; 

        for (const vector<string>& rule : rules) {
            // Compute FIRST(rule)
            unordered_set<char> firstOfRule = computeFirstOfSequence(rule, FIRST);

            // Add entries to the parsing table for each terminal in FIRST(rule)
            for (char terminal : firstOfRule) {
                if (terminal != '~') { // Skip epsilon
                    // Convert the rule to a string for the table entry
                    string ruleStr;
                    for (const string& symbol : rule) {
                        ruleStr += symbol + " ";
                    }
                    ruleStr.pop_back(); // Remove the trailing space
                    parsingTable[nonTerminal][terminal] = nonTerminal + " -> " + ruleStr;
                }
            }

            // If FIRST(rule) contains epsilon, add entries for each terminal in FOLLOW(nonTerminal)
            if (firstOfRule.count('~')) {
                for (char terminal : FOLLOW.at(nonTerminal)) {
                    // Convert the rule to a string for the table entry
                    string ruleStr;
                    for (const string& symbol : rule) {
                        ruleStr += symbol + " ";
                    }
                    ruleStr.pop_back(); // Remove the trailing space
                    parsingTable[nonTerminal][terminal] = nonTerminal + " -> " + ruleStr;
                }
            }
        }
    }

}

#endif