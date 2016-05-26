CC = gcc
CFLAGS=-Wall -lm -g

publicKey:
	$(CC) -static -L/usr/lib -I/usr/lib publicKey.c $(CFLAGS) -o publicKey -ltommath

clean:
	rm -rf decrypted
	rm -rf output
	rm -rf publicKey
