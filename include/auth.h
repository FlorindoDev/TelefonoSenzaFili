#ifndef AUTH_H
#define AUTH_H

typedef struct Utente{
    char nome[50];
    char password[50];
    char lingua[50];
    char funzione[50];
    struct Utente * next;
}Utente;

enum Stato {
    CREATA,
    INIZIATA,
    SOSPESA,
    FINITA,
    CHIUSA
};

typedef struct Stanza{
    char nomeStanza[50];
    enum Stato stato;
    Utente * listaPartecipanti;
    struct Stanza * next;
    
}Stanza;


int login(PGconn *conn,Utente*);

int register_user(PGconn *conn,Utente*);

#include "auth.c"

#endif