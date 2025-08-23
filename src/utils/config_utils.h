#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#include "cjson/cJSON.h"

struct ConfigService {
    cJSON *(*get_config)();
    void (*free_config)();
};

extern const struct ConfigService config_service;

#endif // CONFIG_UTILS_H
