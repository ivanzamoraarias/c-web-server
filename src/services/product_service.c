#include "product_service.h"
#include <openssl/ssl.h>
#include <string.h>
#include <cjson/cJSON.h>

// Simple product structure
typedef struct {
    int id;
    const char *name;
    double price;
} Product;

// Example data
static Product products[] = {
    {1, "Laptop", 699.99},
    {2, "Mouse", 24.95},
    {3, "Keyboard", 49.50},
};
static const int product_count = sizeof(products)/sizeof(products[0]);

int handle_get_product(SSL *ssl, const char *request, int request_len) {
    // Build JSON response using cJSON
    cJSON *json_array = cJSON_CreateArray();
    for (int i = 0; i < product_count; ++i) {
        cJSON *item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "id", products[i].id);
        cJSON_AddStringToObject(item, "name", products[i].name);
        cJSON_AddNumberToObject(item, "price", products[i].price);
        cJSON_AddItemToArray(json_array, item);
    }
    char *json_str = cJSON_PrintUnformatted(json_array);

    // Send HTTP response
    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        strlen(json_str));
    SSL_write(ssl, header, strlen(header));
    SSL_write(ssl, json_str, strlen(json_str));

    // Clean up
    cJSON_Delete(json_array);
    free(json_str);
    return 0;
}

const struct ProductService product_service = {
    .handle_get_product = handle_get_product
};