#pragma once

#include <openssl/ssl.h>

// Handler for GET /products
int handle_get_product(SSL *ssl, const char *request, int request_len);