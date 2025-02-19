# Lexical Analyzer for Custom Programming Language

## Introduction
This project is a lexical analyzer for a custom programming language, implemented in Java using regular expressions and data structures. The analyzer classifies tokens into different categories, including keywords, datatypes, variables, operators, and symbols. It also detects lexical errors and maintains a symbol table. The project was developed using **IntelliJ IDEA**.

## Features
- Tokenization of a custom programming language
- Identification of valid **datatypes, keywords, operators, and variables**
- Detection of **lexical errors**
- Maintenance of a **symbol table**
- Handling of **single-line and multi-line comments**
- Support for **scope-based variable classification (Global & Local)**

## Setup & Compilation
### Prerequisites
- Java Development Kit (JDK) 8 or later
- IntelliJ IDEA (recommended)
- A terminal or command prompt for running the Java program

### Installation Steps
1. Clone the repository:
   ```sh
   git clone <repository_url>
   ```
2. Open the project in **IntelliJ IDEA**.
3. Compile and run the `Tokenization.java` file.
4. Provide input code as a string inside the `main` method or modify it to read from a file.

## File Structure
- **`DFA.java`** - Defines deterministic finite automata (DFA) rules for token validation.
- **`Tokenization.java`** - Handles tokenization and classification of input code.
- **`SymbolTable.java`** - Maintains and displays the symbol table.
- **`LexicalErrorTable.java`** - Stores and displays lexical errors.

## Language Syntax & Rules
### Data Types
| Data Type         | Example       |
|-------------------|--------------|
| Integer          | `123`         |
| Floating Point   | `12.345`      |
| Character        | `'A'`         |
| Boolean         | `Sach`, `Jhoot` |

### Keywords
| Keyword           | Description              |
|-------------------|--------------------------|
| `START_MAIN()`   | Entry point of program   |
| `Integer`        | Integer datatype         |
| `FloatingPoint`  | Floating point datatype  |
| `Character`      | Character datatype       |
| `Boolean`        | Boolean datatype         |

### Variable Naming Rules
- Must start with a **lowercase** letter.
- Can contain **numbers**, **dashes (-)**, and **emojis**.
- Example: `var1`, `abc-123`, `name🙂`

### Operators
| Operator  | Meaning         |
|----------|----------------|
| `+`      | Addition       |
| `-`      | Subtraction    |
| `*`      | Multiplication |
| `/`      | Division       |
| `%`      | Modulus        |
| `^`      | Exponent       |
| `=`      | Assignment     |

### Brackets & Comments
| Type             | Symbol     | Example |
|-----------------|-----------|---------|
| Parentheses     | `()`      | `(x + y)` |
| Curly Braces    | `{}`      | `{ code }` |
| Square Brackets | `[]`      | `[1, 2, 3]` |
| Single-Line Comment | `:>` | `:> This is a comment` |
| Multi-Line Comment | `:< ... >:` | `:< Multi-line >:` |

## Example Program
```java
START_MAIN() {
    Integer x = 10
    FloatingPoint y = 10.54321
    Character c = 'A'
    Boolean flag = Sach
    :> This is a single-line comment
    :< This is a multi-line comment >:
    x = x + y
}
```

## How It Works
1. **Tokenization:** The program reads the source code line by line and splits it into tokens.
2. **Validation:** Each token is validated using the `DFA` class.
3. **Symbol Table:** Recognized tokens (variables, keywords, datatypes) are stored in the `SymbolTable`.
4. **Error Detection:** Unrecognized tokens and incorrect syntax are stored in the `LexicalErrorTable`.
5. **Output Display:** The program prints the **symbol table** and **lexical errors** if any.

## Running the Program
Run the `Tokenization` class, which contains a sample input code. The output will display:
1. **Recognized tokens** in the symbol table.
2. **Lexical errors**, if any.

## Notes
- This implementation was **developed using IntelliJ IDEA**.
- Ensure that the input code follows the syntax rules for correct parsing.

## Author
This project was created as part of an assignment. Please ensure all **rules, syntax, and features** are clearly understood before implementation.

## License
This project is for educational purposes only and should not be distributed without permission.
