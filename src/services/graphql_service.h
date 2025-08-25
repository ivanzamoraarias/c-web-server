#pragma once

#include <openssl/ssl.h>

struct GraphqlService
{
    int (*handle_graphql)(SSL *ssl, const char *reqbuf, int req_len);
};


extern const struct GraphqlService graphql_service;


