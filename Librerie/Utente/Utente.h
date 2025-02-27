#ifndef UTENTE_H
#define UTENTE_H

#define MAX_UTENTE 50

#include <pthread.h>
//#include "../Stanze/Stanze.h"

typedef struct Utente{
    char nome[MAX_UTENTE];
    char password[MAX_UTENTE];
    char lingua[MAX_UTENTE];
    char funzione[MAX_UTENTE];
    int socket;
    pthread_t thread;
    struct Utente * next;
    struct Utente * prev;
}Utente;

void initUtente(Utente * utente, char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE], int socket);

int getUserSocket(Utente * utente);

void printUtente(Utente * utente);

/*
da il next di Utente
- `Utente*`: utente da voler vedere il next 
`return` il next
*/
Utente * getNext(Utente *);

void setPthread(Utente *, pthread_t);

int stampaLista(Utente*);

#include "Utente.c"

#endif


