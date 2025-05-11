#include "csvgen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char table_name[64];
    FILE *output_file;
    int has_header;
    int row_counter;
} CsvTable;

#define MAX_TABLES 100
static CsvTable tables[MAX_TABLES];
static int active_table_count = 0;

static CsvTable* get_csv_table(const char *name) {
    for (int i = 0; i < active_table_count; i++) {
        if (strcmp(tables[i].table_name, name) == 0) {
            return &tables[i];
        }
    }

    CsvTable *new_table = &tables[active_table_count++];
    strncpy(new_table->table_name, name, sizeof(new_table->table_name) - 1);
    
    char filename[128];
    snprintf(filename, sizeof(filename), "%s.csv", name);
    new_table->output_file = fopen(filename, "w");
    if (!new_table->output_file) {
        perror("Error creating CSV file");
        exit(EXIT_FAILURE);
    }
    new_table->has_header = 0;
    new_table->row_counter = 1;
    return new_table;
}

static void write_csv_header(CsvTable *table, JsonNode *object, const char *foreign_key) {
    if (table->has_header) return;
    
    fprintf(table->output_file, "id");
    if (foreign_key) fprintf(table->output_file, ",%s_id", foreign_key);

    for (int i = 0; i < object->object_data.property_count; i++) {
        const char *key = object->object_data.properties[i]->property_data.key;
        JsonNode *value = object->object_data.properties[i]->property_data.value;
        
        if (value->node_type != JN_OBJECT && value->node_type != JN_ARRAY) {
            fprintf(table->output_file, ",%s", key);
        } else if (value->node_type == JN_OBJECT) {
            fprintf(table->output_file, ",%s_id", key);
        }
    }
    
    fprintf(table->output_file, "\n");
    table->has_header = 1;
}

static void write_csv_row(CsvTable *table, JsonNode *object, int row_id, int parent_id, const char *foreign_key) {
    fprintf(table->output_file, "%d", row_id);
    if (foreign_key) fprintf(table->output_file, ",%d", parent_id);

    for (int i = 0; i < object->object_data.property_count; i++) {
        JsonNode *value = object->object_data.properties[i]->property_data.value;
        
        switch (value->node_type) {
            case JN_STRING:
                fprintf(table->output_file, ",%s", value->string_val);
                break;
            case JN_NUMBER:
                fprintf(table->output_file, ",%s", value->number_val);
                break;
            case JN_BOOLEAN:
                fprintf(table->output_file, ",%s", value->boolean_val ? "true" : "false");
                break;
            case JN_NULL:
                fprintf(table->output_file, ",");
                break;
            case JN_OBJECT:
                fprintf(table->output_file, ",%s", value->number_val);
                break;
            default:
                fprintf(table->output_file, ",");
                break;
        }
    }
    
    fprintf(table->output_file, "\n");
}

static int process_json_object(JsonNode *object, const char *table_name, int parent_id, const char *foreign_key);

static void process_json_array(JsonNode *array, const char *name, int parent_id) {
    for (int i = 0; i < array->array_data.element_count; i++) {
        JsonNode *element = array->array_data.elements[i];
        
        if (element->node_type == JN_OBJECT) {
            process_json_object(element, name, parent_id, "parent");
        } else {
            CsvTable *table = get_csv_table(name);
            
            if (!table->has_header) {
                fprintf(table->output_file, "parent_id,index,value\n");
                table->has_header = 1;
            }
            
            fprintf(table->output_file, "%d,%d,", parent_id, i);
            
            switch (element->node_type) {
                case JN_STRING:
                    fprintf(table->output_file, "%s", element->string_val);
                    break;
                case JN_NUMBER:
                    fprintf(table->output_file, "%s", element->number_val);
                    break;
                case JN_BOOLEAN:
                    fprintf(table->output_file, "%s", element->boolean_val ? "true" : "false");
                    break;
                default:
                    break;
            }
            
            fprintf(table->output_file, "\n");
        }
    }
}

static int process_json_object(JsonNode *object, const char *table_name, int parent_id, const char *foreign_key) {
    CsvTable *table = get_csv_table(table_name);
    int current_id = table->row_counter++;
    
    for (int i = 0; i < object->object_data.property_count; i++) {
        JsonNode *value = object->object_data.properties[i]->property_data.value;
        const char *key = object->object_data.properties[i]->property_data.key;
        
        if (value->node_type == JN_OBJECT) {
            int child_id = process_json_object(value, key, current_id, NULL);
            char id_str[16];
            snprintf(id_str, sizeof(id_str), "%d", child_id);
            object->object_data.properties[i]->property_data.value = create_json_number(id_str);
        } else if (value->node_type == JN_ARRAY) {
            process_json_array(value, key, current_id);
        }
    }
    
    write_csv_header(table, object, foreign_key);
    write_csv_row(table, object, current_id, parent_id, foreign_key);
    return current_id;
}

void convert_json_to_csv(JsonNode *root_node, const char *output_directory) {
    if (root_node && root_node->node_type == JN_OBJECT) {
        process_json_object(root_node, "main", 0, NULL);
    }
}