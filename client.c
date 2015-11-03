#include "common.h"

#define CERTFILE "client.pem"
SSL_CTX *setup_client_ctx(void)
{
    SSL_CTX *ctx;

    ctx = SSL_CTX_new(SSLv23_method());
    if(SSL_CTX_use_certificate_chain_file(ctx, CERTFILE) != 1)
        log_err("Error loading certificate file");
    if(SSL_CTX_use_PrivateKey_file(ctx, CERTFILE, SSL_FILETYPE_PEM) != 1)
        log_err("Error loading private key file");
    return ctx;
}

//void do_client(BIO *conn)
int do_client(SSL *ssl)
{
    int err, wr;
    char buf[80];

    for(;;) {
        if(!fgets(buf, sizeof(buf), stdin)) 
            break;
        for(wr = 0; wr < sizeof(buf); wr += err) {
            //err = BIO_write(conn, buf + wr, strlen(buf) - wr);
            err = SSL_write(ssl, buf + wr, strlen(buf) - wr);
            if(err <= 0)
                return 0;
        }
    }
    return 1;
}

int main(int argc, char **argv)
{
    BIO *conn;
    SSL *ssl;
    SSL_CTX *ctx;

    init_openssl();
    seed_prng(64);

    ctx = setup_client_ctx();

    conn = BIO_new_connect(SERVER ":" PORT);
    if(!conn)
        log_err("Failed create bio connection");

    if(BIO_do_connect(conn) <= 0)
        log_err("Failed connecting to remote host");

    if(!(ssl = SSL_new(ctx)))
        log_err("Error creating an SSL context.");
    SSL_set_bio(ssl, conn, conn);
    if(SSL_connect(ssl) <= 0)
        log_err("Error connecting SSL object.");

    fprintf(stderr, "Connection opened\n");
    if(do_client(ssl))
        SSL_shutdown(ssl);
    else
        SSL_clear(ssl);
    fprintf(stderr, "Connection closed\n");

    //BIO_free(conn);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    return 0;
}


