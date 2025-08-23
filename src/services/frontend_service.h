#ifndef FRONTEND_SERVICE_H
#define FRONTEND_SERVICE_H

#include <openssl/ssl.h>


struct FrontendService {
    int (*serve_frontend)(SSL *ssl, const char *reqbuf, int req_len);
};

extern const struct FrontendService frontend_service;

#endif // FRONTEND_SERVICE_H