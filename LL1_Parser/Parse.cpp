#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>


#define MAX_RULES 100
#define MAX_LEN 256

const char* FILENAME1 = "contextfreegrammar.txt";
const char* FILENAME2 = "processedgrammar.txt";
// Note: The following are the constraints for the grammar.
// 1. The start symbol starts always with S.
// 2. # denotes the epsilon.
// 3. The grammar is space seperated. 
// 4. Terminals like 'id', 'exp' are considered different terminal symbols instead of one.


// -------------------------------------------------------------- Left Factoring and Recursion Removal -----------------------------------------------------------------
typedef struct
{
    char nonTerminal;
    char productions[MAX_RULES][MAX_LEN];
    int prodCount;
} Rule;

int ruleCount = 0;
Rule grammar[MAX_RULES];

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
            strcpy(newRule.productions[newRule.prodCount++], "#");
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

// -------------------------------------------------------------- Parser Related Information -------------------------------------------------------------------------
class LL1Parser {
private:
    // Maps a non-terminal to its productions
    std::map<char, std::vector<std::string>> grammar;

    // Set of all terminals in the grammar
    std::set<char> terminals;

    // Set of all non-terminals in the grammar
    std::set<char> nonTerminals;

    // The FIRST set for each symbol
    std::map<char, std::set<char>> firstSets;

    // The FOLLOW set for each non-terminal
    std::map<char, std::set<char>> followSets;

    // The parsing table: maps (non-terminal, terminal) to production
    std::map<std::pair<char, char>, std::string> parsingTable;

    // Special symbol for epsilon
    const char EPSILON = '#';

    // End of input marker
    const char END_MARKER = '$';

public:
    // Load grammar from file
    bool loadGrammar(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines or comments
            if (line.empty()) { continue; }

            char nonTerminal = line[0];
            nonTerminals.insert(nonTerminal);

            // Split the right side into multiple productions
            std::vector<std::string> productions;
            std::string rightSide = line.substr(4);

            size_t start = 0;
            size_t end = rightSide.find('|');
            while (end != std::string::npos) {
                std::string prod = rightSide.substr(start, end - start);
                // Trim whitespace
                prod.erase(0, prod.find_first_not_of(" \t"));
                prod.erase(prod.find_last_not_of(" \t") + 1);
                productions.push_back(prod);

                // Add terminals to the set
                for (char c : prod) {
                    if (std::islower(c) || (!std::isalpha(c) && c != EPSILON)) {
                        terminals.insert(c);
                    }
                }

                start = end + 1;
                end = rightSide.find('|', start);
            }

            // Add the last production
            std::string prod = rightSide.substr(start);
            prod.erase(0, prod.find_first_not_of(" \t"));
            prod.erase(prod.find_last_not_of(" \t") + 1);
            productions.push_back(prod);

            // Add terminals to the set
            for (char c : prod) {
                if (std::islower(c) || (!std::isalpha(c) && c != EPSILON)) {
                    terminals.insert(c);
                }
            }

            grammar[nonTerminal] = productions;
        }

        // Add end marker to terminals
        terminals.insert(END_MARKER);

        return true;
    }

    // Compute FIRST sets for all symbols
    void computeFirstSets() {
        // Initialize FIRST sets for non-terminals to empty
        for (char nonTerminal : nonTerminals) {
            firstSets[nonTerminal] = std::set<char>();
        }

        bool changed = true;
        while (changed) {
            changed = false;

            // Use traditional iterator instead of structured binding
            for (std::map<char, std::vector<std::string>>::const_iterator grammarIt = grammar.begin(); grammarIt != grammar.end(); ++grammarIt) {
                char nonTerminal = grammarIt->first;
                const std::vector<std::string>& productions = grammarIt->second;

                for (const std::string& production : productions) {
                    size_t origSize = firstSets[nonTerminal].size();

                    // If production is epsilon, add epsilon to FIRST
                    if (production.empty() || production[0] == EPSILON) {
                        if (firstSets[nonTerminal].insert(EPSILON).second) {
                            changed = true;
                        }
                        continue;
                    }

                    // Add FIRST of the first symbol to FIRST of nonTerminal
                    bool allDeriveEpsilon = true;
                    for (size_t i = 0; i < production.size(); i++) {
                        char symbol = production[i];

                        // If it's a terminal, add it and break
                        if (terminals.find(symbol) != terminals.end()) {
                            if (firstSets[nonTerminal].insert(symbol).second) {
                                changed = true;
                            }
                            allDeriveEpsilon = false;
                            break;
                        }

                        // If it's a non-terminal, add all its FIRST elements except epsilon
                        bool hasEpsilon = false;
                        for (char first : firstSets[symbol]) {
                            if (first != EPSILON) {
                                if (firstSets[nonTerminal].insert(first).second) {
                                    changed = true;
                                }
                            }
                            else {
                                hasEpsilon = true;
                            }
                        }

                        // If this symbol doesn't derive epsilon, we're done
                        if (!hasEpsilon) {
                            allDeriveEpsilon = false;
                            break;
                        }

                        // If this is the last symbol and it derives epsilon,
                        // add epsilon to FIRST of nonTerminal
                        if (i == production.size() - 1 && hasEpsilon) {
                            if (firstSets[nonTerminal].insert(EPSILON).second) {
                                changed = true;
                            }
                        }
                    }

                    // If all symbols in production derive epsilon, add epsilon to FIRST of nonTerminal
                    if (allDeriveEpsilon && !production.empty()) {
                        if (firstSets[nonTerminal].insert(EPSILON).second) {
                            changed = true;
                        }
                    }

                    if (firstSets[nonTerminal].size() > origSize) {
                        changed = true;
                    }
                }
            }
        }
    }

    // Compute FOLLOW sets for all non-terminals
    void computeFollowSets() {
        // Add end marker to FOLLOW of start symbol
        followSets['S'].insert(END_MARKER);

        bool changed = true;
        while (changed) {
            changed = false;

            // Iterate through all productions
            for (const auto& grammarPair : grammar) {
                char nonTerminal = grammarPair.first;
                const std::vector<std::string>& productions = grammarPair.second;

                for (const std::string& production : productions) {
                    // For each position in the production
                    for (size_t i = 0; i < production.size(); i++) {
                        char currentSymbol = production[i];

                        if (currentSymbol == ' ')    continue;

                        // Skip if not a non-terminal
                        if (nonTerminals.find(currentSymbol) == nonTerminals.end()) {
                            continue;
                        }

                        // Case 1: There are more symbols after the current one
                        if (i < production.size() - 1) {
                            // Process the remaining part of the production
                            bool allCanDeriveEpsilon = true;

                            // Look at all symbols that follow the current one
                            for (size_t j = i + 1; j < production.size() && allCanDeriveEpsilon; j++) {
                                char nextSymbol = production[j];

                                if (nextSymbol == ' ')    continue;

                                // If terminal, add to FOLLOW and stop processing (can't derive epsilon)
                                if (terminals.find(nextSymbol) != terminals.end()) {
                                    if (followSets[currentSymbol].insert(nextSymbol).second) {
                                        changed = true;
                                    }
                                    allCanDeriveEpsilon = false;
                                    break;
                                }
                                // If non-terminal
                                else if (nonTerminals.find(nextSymbol) != nonTerminals.end()) {
                                    // Add everything from FIRST set except epsilon
                                    bool hasEpsilon = false;
                                    for (char first : firstSets[nextSymbol]) {
                                        if (first != EPSILON) {
                                            if (followSets[currentSymbol].insert(first).second) {
                                                changed = true;
                                            }
                                        }
                                        else {
                                            hasEpsilon = true;
                                        }
                                    }

                                    // If this symbol cannot derive epsilon, stop processing
                                    if (!hasEpsilon) {
                                        allCanDeriveEpsilon = false;
                                        break;
                                    }
                                    // Otherwise continue to next symbol (since this one can be epsilon)
                                }
                            }

                            // If all remaining symbols can derive epsilon, add FOLLOW of LHS
                            if (allCanDeriveEpsilon) {
                                for (char follow : followSets[nonTerminal]) {
                                    if (followSets[currentSymbol].insert(follow).second) {
                                        changed = true;
                                    }
                                }
                            }
                        }
                        // Case 2: This is the last symbol in the production
                        else {
                            // Add FOLLOW of LHS to current symbol's FOLLOW set
                            for (char follow : followSets[nonTerminal]) {
                                if (followSets[currentSymbol].insert(follow).second) {
                                    changed = true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // Construct the LL(1) parsing table
    bool constructParsingTable() {
        // Initialize parsing table
        parsingTable.clear();

        // For each production rule A -> α
        for (std::map<char, std::vector<std::string>>::const_iterator grammarIt = grammar.begin();
            grammarIt != grammar.end(); ++grammarIt) {
            char nonTerminal = grammarIt->first;
            const std::vector<std::string>& productions = grammarIt->second;

            for (const std::string& production : productions) {
                std::set<char> firstOfProduction;

                // If production is epsilon, use FOLLOW(A)
                if (production.empty() || production[0] == EPSILON) {
                    for (char follow : followSets[nonTerminal]) {
                        auto entry = std::make_pair(nonTerminal, follow);

                        // Check for conflicts
                        if (parsingTable.find(entry) != parsingTable.end()) {
                            std::cerr << "Parsing table conflict for [" << nonTerminal << ", " << follow
                                << "]: " << parsingTable[entry] << " vs " << production << std::endl;
                            return false;
                        }

                        parsingTable[entry] = production;
                    }
                    continue;
                }

                // Compute FIRST of the production
                computeFirstOfString(production, firstOfProduction);

                // For each terminal in FIRST(α)
                for (char terminal : firstOfProduction) {
                    if (terminal == EPSILON) {
                        // If epsilon is in FIRST(α), add A -> α to M[A, b] for each b in FOLLOW(A)
                        for (char follow : followSets[nonTerminal]) {
                            auto entry = std::make_pair(nonTerminal, follow);

                            // Check for conflicts
                            if (parsingTable.find(entry) != parsingTable.end()) {
                                std::cerr << "Parsing table conflict for [" << nonTerminal << ", " << follow
                                    << "]: " << parsingTable[entry] << " vs " << production << std::endl;
                                return false;
                            }

                            parsingTable[entry] = production;
                        }
                    }
                    else {
                        // Add A -> α to M[A, terminal]
                        auto entry = std::make_pair(nonTerminal, terminal);

                        // Check for conflicts
                        if (parsingTable.find(entry) != parsingTable.end()) {
                            std::cerr << "Parsing table conflict for [" << nonTerminal << ", " << terminal
                                << "]: " << parsingTable[entry] << " vs " << production << std::endl;
                            return false;
                        }

                        parsingTable[entry] = production;
                    }
                }
            }
        }

        return true;
    }

    // Compute FIRST of a string
    void computeFirstOfString(const std::string& str, std::set<char>& result) {
        if (str.empty()) {
            result.insert(EPSILON);
            return;
        }

        bool allDeriveEpsilon = true;

        for (size_t i = 0; i < str.size(); i++) {
            char symbol = str[i];

            // If symbol is a terminal, add it to result and break
            if (terminals.find(symbol) != terminals.end()) {
                result.insert(symbol);
                allDeriveEpsilon = false;
                break;
            }

            // If symbol is the epsilon character, add epsilon and continue
            if (symbol == EPSILON) {
                result.insert(EPSILON);
                continue;
            }

            // If symbol is a non-terminal, add all its FIRST elements except epsilon
            bool hasEpsilon = false;
            for (char first : firstSets[symbol]) {
                if (first != EPSILON) {
                    result.insert(first);
                }
                else {
                    hasEpsilon = true;
                }
            }

            // If this symbol doesn't derive epsilon, we're done
            if (!hasEpsilon) {
                allDeriveEpsilon = false;
                break;
            }

            // If this is the last symbol and it derives epsilon, add epsilon to result
            if (i == str.size() - 1 && hasEpsilon) {
                result.insert(EPSILON);
            }
        }

        // If all symbols derive epsilon, add epsilon to result
        if (allDeriveEpsilon && !str.empty()) {
            result.insert(EPSILON);
        }
    }

    // Display the grammar
    void printGrammar() {
        std::cout << "Grammar:" << std::endl;
        for (std::map<char, std::vector<std::string>>::const_iterator grammarIt = grammar.begin();
            grammarIt != grammar.end(); ++grammarIt) {
            char nonTerminal = grammarIt->first;
            const std::vector<std::string>& productions = grammarIt->second;

            std::cout << nonTerminal << " -> ";
            for (size_t i = 0; i < productions.size(); i++) {
                std::cout << productions[i];
                if (i < productions.size() - 1) {
                    std::cout << " | ";
                }
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }

    // Display the FIRST sets
    void printFirstSets() {
        std::cout << "FIRST Sets:" << std::endl;
        for (char nonTerminal : nonTerminals) {
            std::cout << "FIRST(" << nonTerminal << ") = { ";
            bool first = true;
            for (char symbol : firstSets[nonTerminal]) {
                if (!first) {
                    std::cout << ", ";
                }
                if (symbol == EPSILON) {
                    std::cout << "#";
                }
                else {
                    std::cout << symbol;
                }
                first = false;
            }
            std::cout << " }" << std::endl;
        }
        std::cout << std::endl;
    }

    // Display the FOLLOW sets
    void printFollowSets() {
        std::cout << "FOLLOW Sets:" << std::endl;
        for (char nonTerminal : nonTerminals) {
            std::cout << "FOLLOW(" << nonTerminal << ") = { ";
            bool first = true;
            for (char symbol : followSets[nonTerminal]) {
                if (!first) {
                    std::cout << ", ";
                }
                if (symbol == END_MARKER) {
                    std::cout << "$";
                }
                else {
                    std::cout << symbol;
                }
                first = false;
            }
            std::cout << " }" << std::endl;
        }
        std::cout << std::endl;
    }

    // Display the parsing table
    // Display the parsing table and save it to a CSV file
    bool saveParsingTableToCSV(const std::string& filename) {
        std::ofstream csvFile(filename);
        if (!csvFile.is_open()) {
            std::cerr << "Error opening file for writing: " << filename << std::endl;
            return false;
        }

        // Write header row with terminals
        csvFile << ","; // Empty cell for the top-left corner
        for (char terminal : terminals) {
            if (terminal == END_MARKER) {
                csvFile << "$";
            }
            else {
                csvFile << terminal;
            }

            // Add comma except for the last terminal
            if (terminal != *terminals.rbegin()) {
                csvFile << ",";
            }
        }
        csvFile << std::endl;

        // Write table rows
        for (char nonTerminal : nonTerminals) {
            csvFile << nonTerminal << ",";

            size_t count = 0;
            for (char terminal : terminals) {
                auto entry = std::make_pair(nonTerminal, terminal);
                if (parsingTable.find(entry) != parsingTable.end()) {
                    std::string production = parsingTable[entry];
                    if (production.empty() || production[0] == EPSILON) {
                        csvFile << "#";
                    }
                    else {
                        csvFile << production;
                    }
                }
                else {
                    csvFile << ""; // Empty cell for no production
                }

                // Add comma except for the last terminal
                count++;
                if (count < terminals.size()) {
                    csvFile << ",";
                }
            }
            csvFile << std::endl;
        }

        csvFile.close();
        std::cout << "Parsing table saved to " << filename << std::endl;
        return true;
    }

    bool parseStringWithStack(const std::string& inputStr) {
        std::vector<char> inputTokens;
        for (char c : inputStr) {
            if (!std::isspace(c)) {
                inputTokens.push_back(c);
            }
        }
        inputTokens.push_back(END_MARKER);  // Add end marker

        std::vector<char> stack;
        stack.push_back(END_MARKER);  // Bottom of stack
        stack.push_back('S');         // Start symbol at top

        size_t tokenIndex = 0;
        bool hasError = false;

        std::cout << "\nParsing Steps:\n";
        std::cout << std::setw(25) << "Stack" << std::setw(25) << "Input" << std::setw(40) << "Action" << std::endl;
        std::cout << std::string(90, '-') << std::endl;

        while (!stack.empty()) {
            // Build stack string
            std::string stackStr;
            for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
                stackStr += *it;
            }

            // Build input string
            std::string remainingInput;
            for (size_t i = tokenIndex; i < inputTokens.size(); i++) {
                remainingInput += (inputTokens[i] == END_MARKER) ? '$' : inputTokens[i];
            }

            std::cout << std::setw(25) << stackStr << std::setw(25) << remainingInput;

            // Get top of stack
            char top = stack.back();
            stack.pop_back();

            // Case 1: End marker
            if (top == END_MARKER) {
                if (tokenIndex >= inputTokens.size() || inputTokens[tokenIndex] == END_MARKER) {
                    std::cout << std::setw(40) << "Accept" << std::endl;
                    return true;
                }
                else {
                    std::cout << std::setw(40) << "Error: Stack empty but input remains" << std::endl;
                    hasError = true;
                    break;
                }
            }
            // Case 2: Terminal
            else if (terminals.find(top) != terminals.end() && top != EPSILON) {
                if (tokenIndex < inputTokens.size() && top == inputTokens[tokenIndex]) {
                    std::cout << std::setw(40) << "Match: " << top << std::endl;
                    tokenIndex++;
                }
                else {
                    std::cout << std::setw(40) << "Error: Expected "
                        << (top == ' ' ? "space" : std::string(1, top))
                        << " but got "
                        << (tokenIndex < inputTokens.size() ? std::string(1, inputTokens[tokenIndex]) : "$")
                        << std::endl;
                    hasError = true;
                    tokenIndex++;  // Skip wrong token to try recovery
                }
            }
            // Case 3: Non-terminal
            else if (nonTerminals.find(top) != nonTerminals.end()) {
                if (tokenIndex < inputTokens.size()) {
                    char input = inputTokens[tokenIndex];
                    auto entry = std::make_pair(top, input);

                    if (parsingTable.find(entry) != parsingTable.end()) {
                        std::string production = parsingTable[entry];

                        if (production.size() == 1 && production[0] == EPSILON) {
                            std::cout << std::setw(40) << "Expand: " << top << " -> ε" << std::endl;
                        }
                        else {
                            std::cout << std::setw(40) << "Expand: " << top << " -> " << production << std::endl;

                            // Push production in reverse, skipping spaces
                            for (auto it = production.rbegin(); it != production.rend(); ++it) {
                                if (!std::isspace(*it)) {  
                                    stack.push_back(*it);
                                }
                            }
                        }
                    }
                    else {
                        std::cout << std::setw(40) << "Error: No rule for " << top << " on input " << input << std::endl;
                        hasError = true;
                        tokenIndex++;  // Skip input to recover
                    }
                }
                else {
                    std::cout << std::setw(40) << "Error: Unexpected end of input" << std::endl;
                    hasError = true;
                    break;
                }
            }
            // Case 4: Epsilon
            else if (top == EPSILON) {
                std::cout << std::setw(40) << "Skip epsilon" << std::endl;
            }
            else {
                std::cout << std::setw(40) << "Error: Unknown symbol on stack" << std::endl;
                hasError = true;
            }
        }

        // Check final success
        if (hasError || tokenIndex != inputTokens.size() - 1) { return false;   }
        else {  return true;    }
    }


    // Process an input file with multiple lines to parse
    bool parseInputFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error opening input file: " << filename << std::endl;
            return false;
        }

        int lineNumber = 0;
        int errorCount = 0;
        std::string line;

        while (std::getline(file, line)) {
            lineNumber++;

            if (!parseStringWithStack(line)) {
                std::cout << "Syntax Error in line " << lineNumber << std::endl;
                errorCount++;
            }
            else {
                std::cout << "Line " << lineNumber << " parsed successfully" << std::endl;
            }
        }

        return errorCount == 0;
    }
};

int main() {
    loadGrammar(FILENAME1);

    printf("Original Grammar:\n");
    displayGrammar();

    removeLeftRecursion();
    printf("\nGrammar after Left Recursion Removal:\n");
    displayGrammar();

    leftFactor();
    printf("\nGrammar after Left Factoring:\n");
    displayGrammar();

    // Put the grammar in a text file.
    writeGrammarToFile(FILENAME2);

    // Parser related work: computing first and follow sets, constructing parsing table, parsing a grammar.
    LL1Parser parser;

    std::string filename = "processedgrammar.txt";

    if (!parser.loadGrammar(filename)) {
        std::cerr << "Failed to load grammar from file." << std::endl;
        return 1;
    }

    parser.printGrammar();
    
    // Compute first sets.
    parser.computeFirstSets();
    parser.printFirstSets();
    
    // Compute follow sets.
    parser.computeFollowSets();
    parser.printFollowSets();

    // Construct the parsing table.
    parser.constructParsingTable();
    parser.saveParsingTableToCSV("parsing_table.csv");

    // Perform the parsing using stack.
    parser.parseInputFile("input.txt");

    return 0;
}