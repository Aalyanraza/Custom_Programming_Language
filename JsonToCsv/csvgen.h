#ifndef CSV_GENERATOR_H
#define CSV_GENERATOR_H

#include "ast.h"

void convert_json_to_csv(JsonNode *root_node, const char *output_directory);

#endif