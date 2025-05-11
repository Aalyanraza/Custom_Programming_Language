#include "ast.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

JsonNode* create_json_string(const char *value) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_STRING;
    node->string_val = strdup(value);
    return node;
}

JsonNode* create_json_number(const char *value) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_NUMBER;
    node->number_val = strdup(value);
    return node;
}

JsonNode* create_json_boolean(int value) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_BOOLEAN;
    node->boolean_val = value;
    return node;
}

JsonNode* create_json_null() {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_NULL;
    return node;
}

JsonNode* create_json_property(const char *key, JsonNode *value) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_PROPERTY;
    node->property_data.key = strdup(key);
    node->property_data.value = value;
    return node;
}

JsonNode* create_json_object(JsonNode **properties, int count) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_OBJECT;
    node->object_data.properties = properties;
    node->object_data.property_count = count;
    return node;
}

JsonNode* create_json_array(JsonNode **elements, int count) {
    JsonNode *node = malloc(sizeof(JsonNode));
    node->node_type = JN_ARRAY;
    node->array_data.elements = elements;
    node->array_data.element_count = count;
    return node;
}

static void print_indentation(int level) {
    for (int i = 0; i < level; i++) printf("  ");
}

void print_json_tree(JsonNode *node, int indent_level) {
    if (!node) return;

    switch (node->node_type) {
        case JN_STRING:
            print_indentation(indent_level);
            printf("STRING: \"%s\"\n", node->string_val);
            break;
            
        case JN_NUMBER:
            print_indentation(indent_level);
            printf("NUMBER: %s\n", node->number_val);
            break;
            
        case JN_BOOLEAN:
            print_indentation(indent_level);
            printf("BOOLEAN: %s\n", node->boolean_val ? "true" : "false");
            break;
            
        case JN_NULL:
            print_indentation(indent_level);
            printf("NULL\n");
            break;
            
        case JN_PROPERTY:
            print_indentation(indent_level);
            printf("PROPERTY: \"%s\"\n", node->property_data.key);
            print_json_tree(node->property_data.value, indent_level + 1);
            break;
            
        case JN_OBJECT:
            print_indentation(indent_level);
            printf("OBJECT (%d properties):\n", node->object_data.property_count);
            for (int i = 0; i < node->object_data.property_count; i++) {
                print_json_tree(node->object_data.properties[i], indent_level + 1);
            }
            break;
            
        case JN_ARRAY:
            print_indentation(indent_level);
            printf("ARRAY (%d elements):\n", node->array_data.element_count);
            for (int i = 0; i < node->array_data.element_count; i++) {
                print_json_tree(node->array_data.elements[i], indent_level + 1);
            }
            break;
    }
}

void free_json_tree(JsonNode *node) {
    if (!node) return;

    switch (node->node_type) {
        case JN_STRING:
            free(node->string_val);
            break;
            
        case JN_NUMBER:
            free(node->number_val);
            break;
            
        case JN_PROPERTY:
            free(node->property_data.key);
            free_json_tree(node->property_data.value);
            break;
            
        case JN_OBJECT:
            for (int i = 0; i < node->object_data.property_count; i++) {
                free_json_tree(node->object_data.properties[i]);
            }
            free(node->object_data.properties);
            break;
            
        case JN_ARRAY:
            for (int i = 0; i < node->array_data.element_count; i++) {
                free_json_tree(node->array_data.elements[i]);
            }
            free(node->array_data.elements);
            break;
            
        default:
            break;
    }

    free(node);
}