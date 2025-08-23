#include "product_service.h"
#include <openssl/ssl.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <libpq-fe.h>

// Simple product structure
typedef struct {
    int id;
    const char *name;
    double price;
} Product;


Product* fetch_products_from_db(int *out_count) {
    PGconn *conn = PQconnectdb("host=localhost dbname=mydb user=postgres password=secret");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        *out_count = 0;
        return NULL;
    }

    PGresult *res = PQexec(conn, "SELECT id, name, price FROM products");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        fprintf(stderr, "SELECT failed: %s", PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        *out_count = 0;
        return NULL;
    }

    int rows = PQntuples(res);
    Product *products = malloc(sizeof(Product) * rows);
    if (!products) {
        fprintf(stderr, "Memory allocation failed\n");
        PQclear(res);
        PQfinish(conn);
        *out_count = 0;
        return NULL;
    }
    for (int i = 0; i < rows; i++) {
        products[i].id = atoi(PQgetvalue(res, i, 0));
        // Copy name string
        const char *name_src = PQgetvalue(res, i, 1);
        products[i].name = strdup(name_src);
        products[i].price = atof(PQgetvalue(res, i, 2));
    }
    *out_count = rows;
    PQclear(res);
    PQfinish(conn);
    return products;
}


int handle_get_product(SSL *ssl, const char *request, int request_len) {
    int product_count = 0;
    Product *products = fetch_products_from_db(&product_count);
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
    for (int i = 0; i < product_count; ++i) {
        free((void*)products[i].name);
    }
    free(products);
    cJSON_Delete(json_array);
    free(json_str);
    return 0;
}

const struct ProductService product_service = {
    .handle_get_product = handle_get_product
};