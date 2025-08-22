#include "https_service.h"
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 8443
#define CERT_FILE "server.crt"
#define KEY_FILE  "server.key"

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

    // Serve frontend/index.html as the response
    FILE *fp = fopen("frontend/index.html", "rb");
    if (!fp) {
        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found\n";
        SSL_write(ssl, not_found, strlen(not_found));
    } else {
        fseek(fp, 0, SEEK_END);
        long filesize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        char *html = malloc(filesize + 1);
        if (html) {
            fread(html, 1, filesize, fp);
            html[filesize] = '\0';
            fclose(fp);
            char header[256];
            snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: %ld\r\n"
                "Connection: close\r\n"
                "\r\n",
                filesize);
            SSL_write(ssl, header, strlen(header));
            SSL_write(ssl, html, filesize);
            free(html);
        } else {
            fclose(fp);
            const char *server_error =
                "HTTP/1.1 500 Internal Server Error\r\n"
                "Content-Type: text/plain\r\n"
                "Connection: close\r\n"
                "\r\n"
                "500 Internal Server Error\n";
            SSL_write(ssl, server_error, strlen(server_error));
        }
    }

    SSL_shutdown(ssl);
    SSL_free(ssl);
    close(client);
    free(cinfo);
    pthread_exit(NULL);
}

void start_https_server() {
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

    SSL_CTX *ctx = SSL_CTX_new(TLS_server_method());
    if (!ctx) {
        fprintf(stderr, "Error creating SSL context\n");
        return;
    }

    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0 ||
        SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
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
    addr.sin_port = htons(PORT);
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

    printf("HTTPS server running on port %d\n", PORT);

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