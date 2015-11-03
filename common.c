#include "common.h"

void handle_error(const char *file, int lineno, const char *msg)
{
    fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
    exit(-1);
}

void init_openssl(void)
{
    if(!SSL_library_init()) {
        fprintf(stderr, "openssl init error\n");
        exit(-1);
    }
    SSL_load_error_strings();
}


