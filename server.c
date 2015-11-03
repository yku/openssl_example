#include "common.h"

#define CERTFILE "server.pem"
SSL_CTX *setup_server_ctx(void)
{
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(SSLv23_method());
    if(SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1)
        log_err("Error loading certificate from file.");
    if(SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1)
        log_err("Error loading private key from file.");
    return ctx;
}

//void do_server(BIO *conn)
int do_server(SSL *ssl)
{
    int err, rd;
    char buf[80];

    do {
        for(rd = 0; rd < sizeof(buf); rd += err) {
            err = SSL_read(ssl, buf+rd, sizeof(buf) - rd);
            if(err <= 0)
                break;
        }
        fwrite(buf, 1, rd, stdout);
    } while(err > 0);
    if(SSL_get_shutdown(ssl) & SSL_RECEIVED_SHUTDOWN)
        return 1;
    else
        return 0;
}

void* server_thread(void *arg)
{
    SSL *ssl = (SSL *)arg;

    pthread_detach(pthread_self());
    if(SSL_accept(ssl) <= 0)
        log_err("Error accepting SSL connection.");
    fprintf(stderr, "SSL Connection opened.\n");
    if(do_server(ssl))
        SSL_shutdown(ssl);
    else
        SSL_clear(ssl);
    fprintf(stderr, "SSL Connection closed.\n");
    SSL_free(ssl);
    ERR_remove_state(0);
}

int main(int argc, char **argv)
{
    BIO *acc, *client;
    SSL *ssl;
    SSL_CTX *ctx;
    pthread_t tid;

    init_openssl();
    seed_prng(64);

    ctx = setup_server_ctx();
    acc = BIO_new_accept(PORT);
    if(!acc)
        log_err("Error creating server socket.");

    /* first call BIO_do_accept() setup accept BIO */
    if(BIO_do_accept(acc) <= 0)
        log_err("Error binding server socket.");

    for(;;) {
        if(BIO_do_accept(acc) <= 0)
            log_err("Error accepting connection.");
        client = BIO_pop(acc);
        if(!(ssl = SSL_new(ctx)))
            log_err("Error creating SSL context.");
        SSL_set_bio(ssl, client, client);
        pthread_create(&tid, NULL, server_thread, ssl);
    }

    SSL_CTX_free(ctx);
    BIO_free(acc);
    return 0;
}
