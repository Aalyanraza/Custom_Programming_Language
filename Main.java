import java.io.*;
import java.nio.file.*;

public class Main {
    public static void main(String[] args) {
        // File path of the source code file
        String filePath = "src//code.txt";

        try {
            // Read all lines from the file as a single string
            String code = new String(Files.readAllBytes(Paths.get(filePath)));

            // Create Tokenizer instance and process the code
            Tokenization tokenizer = new Tokenization();
            tokenizer.tokenize(code);

            // Display the symbol table
            tokenizer.displayResults();

            // Create and display NFAs and DFAs for different token types
            System.out.println("\n=== NFA and DFA for Variable Pattern ===");
            NFA variableNFA = NFABuilder.buildVariableNFA();
            variableNFA.printTransitionTable();

            DFA variableDFA = variableNFA.convertToDFA();
            variableDFA.printTransitionTable();

            System.out.println("\n=== NFA and DFA for Integer Pattern ===");
            NFA integerNFA = NFABuilder.buildIntegerNFA();
            integerNFA.printTransitionTable();

            DFA integerDFA = integerNFA.convertToDFA();
            integerDFA.printTransitionTable();

            System.out.println("\n=== NFA and DFA for Floating Point Pattern ===");
            NFA floatNFA = NFABuilder.buildFloatNFA();
            floatNFA.printTransitionTable();

            DFA floatDFA = floatNFA.convertToDFA();
            floatDFA.printTransitionTable();

        } catch (IOException e) {
            System.out.println("Error reading the file: " + e.getMessage());
        }
    }
}