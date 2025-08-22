#pragma once

#include <openssl/ssl.h>

// Handler function type for endpoints
typedef int (*endpoint_handler_t)(SSL *ssl, const char *request, int request_len);

// Register an endpoint for a method and path (exact match for simplicity)
void register_endpoint(const char *method, const char *path, endpoint_handler_t handler);

// Start HTTPS server (same as before)
void start_https_server();