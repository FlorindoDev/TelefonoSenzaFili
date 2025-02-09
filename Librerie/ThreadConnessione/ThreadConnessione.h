#ifndef THREADCONNESSIONE_H
#define THREADCONNESSIONE_H


#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define BUFFER_SIZE 2048

/*
    Quello che neccessita un thread per gestire la connessione
*/
typedef struct GestioneConnessioneArgs{
    int * socket;
    char buffer[BUFFER_SIZE];
    Utente utente;

}GestioneConnessioneArgs;

/*
    pulische GestioneConnessioneArgs che è stata creata per quella connessione
*/
void cleanup_handler_connection(void *args);

/*
    Funzione eseguita dal thread
    `NOTA BENE` se implementi `THREADCONNESSIONE_H` dovrai implementare TU questa funzione
*/
void * Thread_GestioneNuovaConnessione(void *args);

/*
    Funzione eseguita dal thread
    `NOTA BENE` se implementi `THREADCONNESSIONE_H` dovrai implementare TU questa funzione
*/
GestioneConnessioneArgs * initArg(int * new_socket);

/*
    Assgena i file descriptor della connessione a un thread
    `NOTA BENE` non è keep a live e il server deve essere gia in asscoloto

    Prende:
    `server_fd`: File descriptor della sokcet del server 
    `address`: è la struttura sockaddr_in che contiene le specidiche del client che si vuole connettere
    `addrlen`: la lungezza di address
*/
void assignConnectionToThread(int server_fd, struct sockaddr_in address, socklen_t addrlen);

GestioneConnessioneArgs * initArgUtente(int *, Utente );


#include "ThreadConnessione.c"

#endif