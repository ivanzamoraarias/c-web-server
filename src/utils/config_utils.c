#include "config_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CONFIG_FILE_PATH "CServerConfig.json"

static cJSON *config = NULL;

cJSON *get_config() {
    if (config != NULL) {
        return config;
    }
    FILE *file = fopen(CONFIG_FILE_PATH, "rb");
    if (!file) {
        fprintf(stderr, "Could not open config file: %s\n", CONFIG_FILE_PATH);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1);
    if (!data) {
        fclose(file);
        fprintf(stderr, "Memory allocation failed for config file\n");
        return NULL;
    }
    fread(data, 1, length, file);
    data[length] = '\0';
    fclose(file);
    config = cJSON_Parse(data);
    free(data);
    if (!config) {
        fprintf(stderr, "Error parsing JSON config\n");
        return NULL;
    }
    return config;
}

void free_config() {
    if (config) {
        cJSON_Delete(config);
        config = NULL;
    }
}
