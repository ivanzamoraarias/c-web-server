#ifndef FRONTEND_SERVICE_H
#define FRONTEND_SERVICE_H

#include <openssl/ssl.h>

// Serves the frontend, given an SSL connection and HTTP request buffer.
// Returns 0 on success, negative on error.
int serve_frontend(SSL *ssl, const char *reqbuf, int req_len);

#endif // FRONTEND_SERVICE_H