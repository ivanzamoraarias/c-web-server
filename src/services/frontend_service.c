#include "frontend_service.h"
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int serve_frontend(SSL *ssl, const char *reqbuf, int req_len) {
    char method[8] = {0};
    char path[256] = {0};
    sscanf(reqbuf, "%7s %255s", method, path);

    // Only handle GET requests
    if (strcmp(method, "GET") != 0) {
        const char *not_allowed =
            "HTTP/1.1 405 Method Not Allowed\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "405 Method Not Allowed\n";
        SSL_write(ssl, not_allowed, strlen(not_allowed));
        return -1;
    }

    // Map path to file in frontend folder
    char file_path[512];
    if (strcmp(path, "/") == 0) {
        snprintf(file_path, sizeof(file_path), "frontend/index.html");
    } else {
        snprintf(file_path, sizeof(file_path), "frontend%s", path);
    }

    // Determine content type
    const char *content_type = "text/html";
    if (strstr(file_path, ".css")) content_type = "text/css";
    else if (strstr(file_path, ".js")) content_type = "application/javascript";
    else if (strstr(file_path, ".txt")) content_type = "text/plain";

    FILE *fp = fopen(file_path, "rb");
    if (!fp) {
        const char *not_found =
            "HTTP/1.1 404 Not Found\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "404 Not Found\n";
        SSL_write(ssl, not_found, strlen(not_found));
        return -2;
    }

    fseek(fp, 0, SEEK_END);
    long filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *filebuf = malloc(filesize + 1);
    if (!filebuf) {
        fclose(fp);
        const char *server_error =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/plain\r\n"
            "Connection: close\r\n"
            "\r\n"
            "500 Internal Server Error\n";
        SSL_write(ssl, server_error, strlen(server_error));
        return -3;
    }

    fread(filebuf, 1, filesize, fp);
    filebuf[filesize] = '\0';
    fclose(fp);

    char header[256];
    snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %ld\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type, filesize);
    SSL_write(ssl, header, strlen(header));
    SSL_write(ssl, filebuf, filesize);
    free(filebuf);

    return 0;
}

const struct FrontendService frontend_service = {
    .serve_frontend = serve_frontend
};