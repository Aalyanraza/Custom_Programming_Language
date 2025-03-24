import java.util.*;

class SymbolEntry {
    String lexeme;   // Actual value (variable, function, etc.)
    String tokenType; // Datatype, Keyword, Variable, Operator
    String scope;    // Global or Local
    int memoryLocation; // Simulated memory address

    public SymbolEntry(String lexeme, String tokenType, String scope) {
        this.lexeme = lexeme;
        this.tokenType = tokenType;
        this.scope = scope;
        this.memoryLocation = lexeme.hashCode(); // Simulated memory location
    }

    @Override
    public String toString() {
        return String.format("| %-15s | %-12s | %-8s | %-14d |", lexeme, tokenType, scope, memoryLocation);
    }
}

class SymbolTable {
    private final Map<String, SymbolEntry> table;

    public SymbolTable() {
        this.table = new LinkedHashMap<>(); // Keeps insertion order
    }

    // Add a new entry to the symbol table
    public void insert(String lexeme, String tokenType, String scope) {
        if (!table.containsKey(lexeme)) {
            table.put(lexeme, new SymbolEntry(lexeme, tokenType, scope));
        }
    }

    // Display the symbol table
    public void display() {
        System.out.println("\nSYMBOL TABLE:");
        System.out.println("| Lexeme         | Token Type   | Scope   | Memory Addr  |");
        for (SymbolEntry entry : table.values()) {
            System.out.println(entry);
        }
    }
}
