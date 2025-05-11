#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "csvgen.h"

// External parser functions and variables
extern int parse_json_input(void);
extern JsonNode *parsed_json_root;

int main(int argc, char *argv[]) {
    // Parse the JSON input
    if (parse_json_input() != 0) {
        fprintf(stderr, "Error: Failed to parse input\n");
        return EXIT_FAILURE;
    }

    printf("JSON parsing completed successfully.\n");

    // Process command line arguments
    int show_ast = 0;
    const char *output_directory = ".";  // Default to current directory
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--show-ast") == 0) {
            show_ast = 1;
        }
        else if (strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_directory = argv[++i];
            } else {
                fprintf(stderr, "Error: --output requires a directory argument\n");
                return EXIT_FAILURE;
            }
        }
    }

    // Display AST if requested
    if (show_ast) {
        printf("\n=== Abstract Syntax Tree ===\n");
        print_json_tree(parsed_json_root, 0);
    }

    // Generate CSV files
    convert_json_to_csv(parsed_json_root, output_directory);
    
    // Clean up memory
    free_json_tree(parsed_json_root);

    return EXIT_SUCCESS;
}