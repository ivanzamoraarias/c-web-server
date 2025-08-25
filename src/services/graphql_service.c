#include "graphql_service.h"
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <cgql/cgql.h> // Make sure cgql is installed and included

void handle_graphql(SSL *ssl, const char *reqbuf, int req_len) {
    // Extract the body (assume it's after two CRLFs)
    const char *body = strstr(reqbuf, "\r\n\r\n");
    if (!body) body = reqbuf;
    else body += 4;

    // Parse JSON body to get the query
    cJSON *json = cJSON_Parse(body);
    const char *query = NULL;
    if (json) {
        cJSON *query_item = cJSON_GetObjectItem(json, "query");
        if (cJSON_IsString(query_item)) {
            query = query_item->valuestring;
        }
    }

    // Parse GraphQL query using cgql
    cgqlResult *result = NULL;
    if (query) {
        cgqlSchema *schema = cgql_createSchema();
        // TODO: Register types and resolvers for products, etc.
        result = cgql_parse(query, schema);
    }

    // Prepare response
    const char *response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/json\r\n"
        "Connection: close\r\n"
        "\r\n"
        "{\"data\":{\"products\":[]}}\n"; // Stub: empty products array

    SSL_write(ssl, response, strlen(response));

    if (json) cJSON_Delete(json);
    if (result) cgql_freeResult(result);
}

const struct GraphqlService graphql_service = {
    .handle_graphql = handle_graphql
};