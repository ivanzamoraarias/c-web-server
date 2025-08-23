#include "https_service.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include "../utils/config_utils.h"

#define DEFAULT_PORT 8442
#define DEFAULT_CERT_FILE "server.crt"
#define DEFAULT_KEY_FILE  "server.key"

// --- Router Implementation ---
typedef struct {
    char method[8];
    char path[256];
    endpoint_handler_t handler;
} endpoint_t;

#define MAX_ENDPOINTS 32
static endpoint_t endpoints[MAX_ENDPOINTS];
static int endpoint_count = 0;

void register_endpoint(const char *method, const char *path, endpoint_handler_t handler) {
    if (endpoint_count < MAX_ENDPOINTS) {
        strncpy(endpoints[endpoint_count].method, method, sizeof(endpoints[endpoint_count].method) - 1);
        strncpy(endpoints[endpoint_count].path, path, sizeof(endpoints[endpoint_count].path) - 1);
        endpoints[endpoint_count].handler = handler;
        endpoint_count++;
    }
}

static endpoint_handler_t find_handler(const char *method, const char *path) {
    for (int i = 0; i < endpoint_count; ++i) {
        if (strcmp(endpoints[i].method, method) == 0 && strcmp(endpoints[i].path, path) == 0) {
            return endpoints[i].handler;
        }
    }
    return NULL;
}

// --- Thread/Connection Handling ---
typedef struct {
    SSL *ssl;
    int client_fd;
} client_info_t;

void *handle_client(void *arg) {
    client_info_t *cinfo = (client_info_t *)arg;
    SSL *ssl = cinfo->ssl;
    int client = cinfo->client_fd;

    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        close(client);
        free(cinfo);
        pthread_exit(NULL);
    }

    char reqbuf[2048];
    int req_len = SSL_read(ssl, reqbuf, sizeof(reqbuf) - 1);
    if (req_len <= 0) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        close(client);
        free(cinfo);
        pthread_exit(NULL);
    }
    reqbuf[req_len] = '\0';

    // Parse method and path
    char method[8] = {0};
    char path[256] = {0};
    sscanf(reqbuf, "%7s %255s", method, path);

    endpoint_handler_t handler = find_handler(method, path);

    if (handler) {
        handler(ssl, reqbuf, req_len);
    } else {
        // Default: 404
        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found\n";
        SSL_write(ssl, not_found, strlen(not_found));
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client);
    free(cinfo);
    pthread_exit(NULL);
}

typedef struct {
    int port;
    const char *cert_file;
    const char *key_file;
} Config;   

void start_https_server() {

    Config server_config;
    server_config.port = DEFAULT_PORT;
    server_config.cert_file = DEFAULT_CERT_FILE;
    server_config.key_file = DEFAULT_KEY_FILE;

    cJSON *config = get_config();
    if (config != NULL) {
        cJSON *port_item = cJSON_GetObjectItem(config, "PORT");
        cJSON *cert_item = cJSON_GetObjectItem(config, "CERT_FILE");
        cJSON *key_item = cJSON_GetObjectItem(config, "KEY_FILE");

        if (port_item && cJSON_IsNumber(port_item))
            server_config.port = port_item->valueint;
        if (cert_item && cJSON_IsString(cert_item))
            server_config.cert_file = cert_item->valuestring;
        if (key_item && cJSON_IsString(key_item))
            server_config.key_file = key_item->valuestring;
    }



   

    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        fprintf(stderr, "Error creating SSL context\n");
        return;
    }

    if (SSL_CTX_use_certificate_file(ctx, server_config.cert_file, SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, server_config.key_file, SSL_FILETYPE_PEM) <= 0) {
        fprintf(stderr, "Error setting up cert/key\n");
        SSL_CTX_free(ctx);
        return;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        SSL_CTX_free(ctx);
        return;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(server_config.port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sockfd);
        SSL_CTX_free(ctx);
        return;
    }

    if (listen(sockfd, 1) < 0) {
        perror("listen");
        close(sockfd);
        SSL_CTX_free(ctx);
        return;
    }

    printf("HTTPS server running on port %d\n", server_config.port);

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t len = sizeof(client_addr);
        int client = accept(sockfd, (struct sockaddr*)&client_addr, &len);
        if (client < 0) {
            perror("accept");
            continue;
        }

        SSL *ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        client_info_t *cinfo = malloc(sizeof(client_info_t));
        if (!cinfo) {
            fprintf(stderr, "Failed to allocate memory for client_info\n");
            SSL_free(ssl);
            close(client);
            continue;
        }
        cinfo->ssl = ssl;
        cinfo->client_fd = client;

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, cinfo) != 0) {
            fprintf(stderr, "Failed to create thread\n");
            SSL_free(ssl);
            close(client);
            free(cinfo);
            continue;
        }
        pthread_detach(tid); // Automatically reclaim thread resources
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
}