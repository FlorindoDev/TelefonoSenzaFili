#ifndef AUTH_H
#define AUTH_H

typedef struct Utente{
    char nome[50];
    char password[50];
    char lingua[50];
    char funzione[50];
    struct Utente * next;
}Utente;

int login(PGconn *conn,Utente*);

int register_user(PGconn *conn,Utente*);

#include "auth.c"

#endif