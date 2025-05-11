#ifndef JSON_AST_H
#define JSON_AST_H

typedef enum {
    JN_OBJECT,
    JN_ARRAY,
    JN_STRING,
    JN_NUMBER,
    JN_BOOLEAN,
    JN_NULL,
    JN_PROPERTY
} JsonNodeType;

typedef struct JsonNode {
    JsonNodeType node_type;
    
    union {
        // For objects
        struct {
            struct JsonNode **properties;
            int property_count;
        } object_data;
        
        // For arrays
        struct {
            struct JsonNode **elements;
            int element_count;
        } array_data;
        
        // For properties (key-value pairs)
        struct {
            char *key;
            struct JsonNode *value;
        } property_data;
        
        // For primitive values
        char *string_val;
        char *number_val;
        int boolean_val;
    };
} JsonNode;

// Creation functions
JsonNode* create_json_string(const char *value);
JsonNode* create_json_number(const char *value);
JsonNode* create_json_boolean(int value);
JsonNode* create_json_null();
JsonNode* create_json_property(const char *key, JsonNode *value);
JsonNode* create_json_object(JsonNode **properties, int count);
JsonNode* create_json_array(JsonNode **elements, int count);

// Utility functions
void print_json_tree(JsonNode *node, int indent_level);
void free_json_tree(JsonNode *node);

#endif