OPENSSL_CNF=/etc/ssl/openssl.cnf

all: common.o
	$(CC) client.c common.o -o client -lssl -lcrypto
	$(CC) server.c common.o -o server -lssl -lcrypto -lpthread

common.o: common.c
	$(CC) -c common.c -lssl -lcrypto

# create root ca
rootca:
	openssl req -newkey rsa:2048 -sha256 -keyout rootkey.pem -out rootreq.pem
	openssl x509 -req -in rootreq.pem -sha256 -extfile $(OPENSSL_CNF) -extensions v3_ca -signkey rootkey.pem -out rootcert.pem
	cat rootcert.pem rootkey.pem > root.pem

# create inter ca
interca:
	openssl req -newkey rsa:2048 -sha256 -keyout intercakey.pem -out intercareq.pem
	openssl x509 -req -in intercareq.pem -sha256 -extfile $(OPENSSL_CNF) -extensions v3_ca -CA root.pem -CAkey root.pem -CAcreateserial -out intercacert.pem
	cat intercacert.pem intercakey.pem rootcert.pem > interca.pem

# create server cert and sign by inter ca
s_cert:
	openssl req -newkey rsa:2048 -sha256 -keyout serverkey.pem -out serverreq.pem
	openssl x509 -req -in serverreq.pem -sha256 -extfile $(OPENSSL_CNF) -extensions usr_cert -CA interca.pem -CAkey interca.pem -CAcreateserial -out servercert.pem
	cat servercert.pem serverkey.pem interca.pem rootcert.pem > server.pem

# create client cert and sign by root ca
c_cert:
	openssl req -newkey rsa:2048 -sha256 -keyout clientkey.pem -out clientreq.pem
	openssl x509 -req -in clientreq.pem -sha256 -extfile $(OPENSSL_CNF) -extensions usr_cert -CA root.pem -CAkey root.pem -CAcreateserial -out clientcert.pem
	cat clientcert.pem clientkey.pem rootcert.pem > client.pem

dh:
	openssl dhparam -check -text -5 512 -out dh512.pem
	openssl dhparam -check -text -5 1024 -out dh1024.pem

clean:
	rm -rf *.o
	rm -rf client server

clean_ca:
	rm -rf *.pem
