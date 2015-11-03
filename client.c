#include "common.h"

void do_client(BIO *conn)
{
    int err, wr;
    char buf[80];

    for(;;) {
        if(!fgets(buf, sizeof(buf), stdin)) 
            break;
        for(wr = 0; wr < sizeof(buf); wr += err) {
            err = BIO_write(conn, buf + wr, strlen(buf) - wr);
            if(err <= 0)
                return;
        }
    }
}

int main(int argc, char **argv)
{
    BIO *conn;

    init_openssl();

    conn = BIO_new_connect(SERVER ":" PORT);
    if(!conn)
        log_err("Failed create bio connection");

    if(BIO_do_connect(conn) <= 0)
        log_err("Failed connecting to remote host");

    fprintf(stderr, "Connection opened\n");
    do_client(conn);
    fprintf(stderr, "Connection closed\n");

    BIO_free(conn);
    return 0;
}


