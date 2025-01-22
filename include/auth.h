#ifndef AUTH_H
#define AUTH_H

typedef struct {
    char* nome;
    char* password;
    char* lingua;
} Utente;

int login(PGconn *conn,Utente*);

int register_user(PGconn *conn,Utente*);

#include "auth.c"

#endif