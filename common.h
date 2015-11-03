#ifndef __COMMON_H__
#define __COMMON_H__

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/ssl.h>
#include <openssl/x509v3.h>
#include <pthread.h>

#define PORT "6510"
#define SERVER "localhost"
#define CLIENT "localhost"

#define log_err(msg) handle_error(__FILE__, __LINE__, msg)
void handle_error(const char *file, int lineno, const char *msg);

void init_openssl(void);

#endif
