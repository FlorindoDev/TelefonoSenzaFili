CC=gcc
CFLAGS=-g -I/usr/include/postgresql
LDFLAGS=-lpq -lpthread

all: my_program client PartitaEXE

my_program: main.c
	$(CC) $(CFLAGS) -o my_program main.c $(LDFLAGS)

client: Client-Server/client.c
	$(CC) -o client Client-Server/client.c $(LDFLAGS)

PartitaEXE: Partita/Partita.c
	$(CC) -o PartitaEXE Partita/Partita.c $(LDFLAGS)

clean:
	rm -f my_program client PartitaEXE