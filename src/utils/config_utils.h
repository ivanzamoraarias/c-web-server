#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include "cjson/cJSON.h"

// Returns a pointer to the loaded config cJSON object (singleton)
cJSON *get_config();

// Frees the config object from memory (call at program exit)
void free_config();

#endif // CONFIG_UTILS_H
