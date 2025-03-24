import java.util.*;

class LexicalErrorTable
{
    private final List<String> errors;

    public LexicalErrorTable() {
        this.errors = new ArrayList<>();
    }

    public void insert(String token) {
        errors.add(token);
    }

    public void display() {
        System.out.println("\nLEXICAL ERRORS:");
        if (errors.isEmpty()) {
            System.out.println("No lexical errors found.");
        } else {
            for (String error : errors) {
                System.out.println("Unrecognized Token: " + error);
            }
        }
    }
}