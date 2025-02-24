CC=gcc
CFLAGS=-g -I/usr/include/postgresql
LDFLAGS=-lpq -lpthread -lcurl -ljansson

all: my_program client PartitaEXE

my_program: main.c
	$(CC) $(CFLAGS) -o my_program main.c $(LDFLAGS)

client: Client-Server/client.c
	$(CC) -o client Client-Server/client.c $(LDFLAGS)

PartitaEXE: Partita/Partita.c
	$(CC) -o PartitaEXE Partita/Partita2.c $(LDFLAGS)

clean:
	rm -f my_program client PartitaEXE