import java.util.*;

class Tokenization {
    private final SymbolTable symbolTable;
    private final LexicalErrorTable errorTable;
    private final Stack<Character> bracketStack;
    private boolean inMultiLineComment;

    public Tokenization() {
        this.symbolTable = new SymbolTable();
        this.errorTable = new LexicalErrorTable();
        this.bracketStack = new Stack<>();
        this.inMultiLineComment = false;
    }

    public void tokenize(String code) {
        for (String line : code.split("\\n")) {
            line = line.trim();

            if (inMultiLineComment) {
                inMultiLineComment = !line.endsWith(">:");
                continue;
            }
            if ((inMultiLineComment = line.startsWith(":<")) || line.startsWith(":>")) continue;

            String[] tokens = line.split("\\s+");

            for (String token : tokens) {
                if(token.isEmpty()) continue;

                char firstChar = token.charAt(0);
                String scope = bracketStack.contains('{') ? "Local" : "Global";

                if (Character.isDigit(firstChar)) {
                    if (RegExpressions.isValidInteger(token)) {
                        symbolTable.insert(token, "Integer", scope);
                    } else if (RegExpressions.isValidFloat(token)) {
                        symbolTable.insert(token, "FloatingPoint", scope);
                    } else {
                        errorTable.insert(token);
                    }
                } else if (Character.isLetter(firstChar)) {
                    if (RegExpressions.isValidDatatype(token) || RegExpressions.isValidMainFunction(token)) {
                        symbolTable.insert(token, "Keyword", scope);
                    } else if (RegExpressions.isValidBoolean(token)) {
                        symbolTable.insert(token, "Boolean", scope);
                    } else if (RegExpressions.isValidVariable(token)) {
                        symbolTable.insert(token, "Variable", scope);
                    } else {
                        errorTable.insert(token);
                    }
                } else {
                    if (RegExpressions.isValidCharacter(token)) {
                        symbolTable.insert(token, "Character", scope);
                    } else if(RegExpressions.isValidOperator(token) ) {
                        symbolTable.insert(token, "Operator", scope);
                    } else if (RegExpressions.isValidParenthesis(token) || RegExpressions.isValidCurlyBrace(token) || RegExpressions.isValidSquareBracket(token)) {
                        symbolTable.insert(token, "Bracket", scope);

                        char bracket = token.charAt(0);
                        if (bracket == '(' || bracket == '{' || bracket == '[') {
                            bracketStack.push(bracket);
                        } else if (bracket == ')' || bracket == '}' || bracket == ']') {
                            if (bracketStack.isEmpty() || !isMatchingPair(bracketStack.pop(), bracket)) {
                                // errorTable.insert("Mismatched bracket: " + token);
                                // Does nothing. Just to check the scope of the variables.
                            }
                        }
                    } else {
                        errorTable.insert(token);
                    }
                }
            }
        }

        if (inMultiLineComment)  {
            errorTable.insert("Multi-line comment not closed");
        }
    }

    private boolean isMatchingPair(char open, char close) {
        return (open == '(' && close == ')') ||
                (open == '{' && close == '}') ||
                (open == '[' && close == ']');
    }

    public void displayResults() {
        symbolTable.display();
        errorTable.display();
    }
}
