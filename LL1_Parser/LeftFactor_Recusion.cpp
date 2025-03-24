#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first_follow_sets.h"
#include "LL1Parser.h"

#define MAX_RULES 100
#define MAX_LEN 256

// NOTE: Assuming the following things from the context free grammar.
// 1. The grammar received is left-factored and non-recursive grammar.
// 2. '~' denotes Epsilon/Null.
// 3. The grammar has all its instances written on single line seperated by '|'.

const char* FILENAME = "contextfreegrammar.txt";
const char* FILENAME_UPDATED = "grammar.txt";
string FILENAME_UPDATED_S = "grammar.txt";

typedef struct
{
    char nonTerminal;
    char productions[MAX_RULES][MAX_LEN];
    int prodCount;
} Rule;

int ruleCount = 0;
Rule grammar[MAX_RULES];

unordered_map<string, unordered_set<char>> FIRST;
unordered_map<string, unordered_set<char>> FOLLOW;
unordered_map<string, unordered_map<char, string>> parsingTable;

// Function to load grammar from a file
void loadGrammar(const char* filename)
{
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    while (fscanf(file, " %c -> %[^\n]", &grammar[ruleCount].nonTerminal, grammar[ruleCount].productions[0]) == 2)
    {
        char* token = strtok(grammar[ruleCount].productions[0], "|");
        while (token)
        {
            strcpy(grammar[ruleCount].productions[grammar[ruleCount].prodCount++], token);
            token = strtok(NULL, "|");
        }
        ruleCount++;
    }
    fclose(file);
}

// Function to check for left recursion
int hasLeftRecursion(Rule* rule) {
    for (int i = 0; i < rule->prodCount; i++) {
        if (rule->productions[i][0] == rule->nonTerminal) {
            return 1; // Left recursion exists
        }
    }
    return 0;
}

// Function to remove left recursion
void removeLeftRecursion() {
    for (int i = 0; i < ruleCount; i++) {
        if (hasLeftRecursion(&grammar[i])) {
            Rule newRule;
            newRule.nonTerminal = grammar[i].nonTerminal + 'A' - 'A' + 1;
            newRule.prodCount = 0;

            Rule original = grammar[i];
            grammar[i].prodCount = 0;

            for (int j = 0; j < original.prodCount; j++) {
                if (original.productions[j][0] == original.nonTerminal) {
                    sprintf(newRule.productions[newRule.prodCount++], "%s%c'", original.productions[j] + 1, newRule.nonTerminal);
                }
                else {
                    sprintf(grammar[i].productions[grammar[i].prodCount++], "%s%c'", original.productions[j], newRule.nonTerminal);
                }
            }
            strcpy(newRule.productions[newRule.prodCount++], "~");
            grammar[ruleCount++] = newRule;
        }
    }
}

// Function to perform left factoring
void leftFactor() {
    for (int i = 0; i < ruleCount; i++) {
        for (int j = 0; j < grammar[i].prodCount - 1; j++) {
            for (int k = j + 1; k < grammar[i].prodCount; k++) {
                int prefixLen = 0;
                while (grammar[i].productions[j][prefixLen] != '\0' &&
                    grammar[i].productions[j][prefixLen] == grammar[i].productions[k][prefixLen]) {
                    prefixLen++;
                }

                if (prefixLen > 0) {
                    Rule newRule;
                    newRule.nonTerminal = grammar[i].nonTerminal + 'A' - 'A' + 1;
                    newRule.prodCount = 0;

                    char commonPrefix[MAX_LEN];
                    strncpy(commonPrefix, grammar[i].productions[j], prefixLen);
                    commonPrefix[prefixLen] = '\0';

                    sprintf(grammar[i].productions[j], "%s%c'", commonPrefix, newRule.nonTerminal);
                    sprintf(newRule.productions[newRule.prodCount++], "%s", grammar[i].productions[j] + prefixLen);
                    sprintf(newRule.productions[newRule.prodCount++], "%s", grammar[i].productions[k] + prefixLen);

                    grammar[i].productions[k][0] = '\0'; // Mark as processed

                    grammar[ruleCount++] = newRule;
                }
            }
        }
    }
}

// Function to display the grammar
void displayGrammar() {
    for (int i = 0; i < ruleCount; i++) {
        printf("%c -> ", grammar[i].nonTerminal);
        for (int j = 0; j < grammar[i].prodCount; j++) {
            if (grammar[i].productions[j][0] != '\0') {
                printf("%s", grammar[i].productions[j]);
                if (j < grammar[i].prodCount - 1) printf(" | ");
            }
        }
        printf("\n");
    }
}

void writeGrammarToFile(const char* filename)
{
    FILE* file = fopen(filename, "w");
    if (!file)
    {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < ruleCount; i++)
    {
        fprintf(file, "%c -> ", grammar[i].nonTerminal);
        for (int j = 0; j < grammar[i].prodCount; j++)
        {
            if (grammar[i].productions[j][0] != '\0')
            {
                fprintf(file, "%s", grammar[i].productions[j]);
                if (j < grammar[i].prodCount - 1)
                    fprintf(file, " | ");
            }
        }
        fprintf(file, "\n");
    }

    fclose(file);
    printf("\nTransformed grammar written to: %s\n", filename);
}

void printFirstSet() {
    cout << "\nFIRST Sets:\n";
    for (const auto& entry : FIRST) {
        if (islower(entry.first[0]) || entry.first == "~") continue;

        cout << "FIRST(" << entry.first << ") = { ";
        for (char terminal : entry.second) {
            cout << terminal << " ";
        }
        cout << "}\n";
    }
}

void printFollowSet() {
    cout << "\nFOLLOW Sets:\n";
    for (const auto& entry : FOLLOW) {
        cout << "FOLLOW(" << entry.first << ") = { ";
        for (char terminal : entry.second) {
            cout << terminal << " ";
        }
        cout << "}\n";
    }
}

int main(int argc, char* argv[])
{
    loadGrammar(FILENAME);

    printf("Original Grammar:\n");
    displayGrammar();

    removeLeftRecursion();
    printf("\nGrammar after Left Recursion Removal:\n");
    displayGrammar();

    leftFactor();
    printf("\nGrammar after Left Factoring:\n");
    displayGrammar();

    // Put the grammar in a text file.
    writeGrammarToFile(FILENAME_UPDATED);

    // Read the updated grammar from the file.
    unordered_map<string, vector<vector<string>>> productions = readGrammar(FILENAME_UPDATED_S);

    // Compute FIRST sets
    for (const auto& it : productions) {
        computeFirst(FIRST, it.first, productions);
    }
    printFirstSet();

    // Compute FOLLOW sets
    computeFollow(FOLLOW, FIRST, productions.begin()->first, productions);
    printFollowSet();

    // Construct the LL1-Parsing Table.
    constructLL1Table(parsingTable, FIRST, FOLLOW, productions);

    cout << "\nLL(1) Parsing Table:\n";
    for (const auto& row : parsingTable) {
        const string& nonTerminal = row.first;
        const unordered_map<char, string>& entries = row.second;
        cout << "Non-terminal: " << nonTerminal << endl;
        for (const auto& entry : entries) {
            char terminal = entry.first;
            const string& production = entry.second;
            cout << "  On terminal '" << terminal << "': " << production << endl;
        }
    }

    return EXIT_SUCCESS;
}
