

all: common.o
	$(CC) client.c common.o -o client -lssl -lcrypto
	$(CC) server.c common.o -o server -lssl -lcrypto -lpthread

common.o: common.c
	$(CC) -c common.c -lssl -lcrypto



clean:
	rm -rf *.o
	rm -rf client server
