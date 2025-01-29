#ifndef AUTH_H
#define AUTH_H

#include <string.h>
#include "../Utente.h"

int login(PGconn *conn,Utente*);

int register_user(PGconn *conn,Utente*);

#include "auth.c"

#endif