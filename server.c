#include "common.h"

void do_server(BIO *conn)
{
    int err, rd;
    char buf[80];

    do {
        for(rd = 0; rd < sizeof(buf); rd += err) {
            err = BIO_read(conn, buf+rd, sizeof(buf) - rd);
            if(err <= 0)
                break;
        }
        fwrite(buf, 1, rd, stdout);
    } while(err > 0);
}

void* server_thread(void *arg)
{
    BIO *client = (BIO *)arg;

    pthread_detach(pthread_self());
    fprintf(stderr, "Connection opened.\n");
    do_server(client);
    fprintf(stderr, "Connection closed.\n");

    BIO_free(client);
    ERR_remove_state(0);
}

int main(int argc, char **argv)
{
    BIO *acc, *client;
    pthread_t tid;

    init_openssl();

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
        pthread_create(&tid, NULL, server_thread, client);
    }

    BIO_free(acc);
    return 0;
}
