#ifndef UTENTE_H
#define UTENTE_H

#define MAX_UTENTE 50

typedef struct Utente{
    char nome[MAX_UTENTE];
    char password[MAX_UTENTE];
    char lingua[MAX_UTENTE];
    char funzione[MAX_UTENTE];
    int socket;
    struct Utente * next;
    struct Utente * prev;
}Utente;

void initUtente(Utente * utente, char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE]);

#include "Utente.c"

#endif


