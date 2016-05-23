CC = gcc
CFLAGS=-Wall -lm

publicKey:
	$(CC) publicKey.c $(CFLAGS) -o publicKey

clean:
	rm -rf publicKey
