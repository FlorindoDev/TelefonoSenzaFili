#ifndef UTENTE_H
#define UTENTE_H

typedef struct Utente{
    char nome[50];
    char password[50];
    char lingua[50];
    char funzione[50];
    struct Utente * next;
}Utente;

#endif


