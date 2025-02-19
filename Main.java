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

        } catch (IOException e) {
            System.out.println("Error reading the file: " + e.getMessage());
        }
    }
}
