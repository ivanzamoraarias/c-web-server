#pragma once

#include <openssl/ssl.h>

struct ProductService
{
    int (*handle_get_product)(SSL *ssl, const char *request, int request_len);
    
};

extern const struct ProductService product_service;
