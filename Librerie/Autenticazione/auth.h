#ifndef AUTH_H
#define AUTH_H

#include <string.h>
#include "../Utente/Utente.h"

int login(PGconn *,Utente*, char *);

int register_user(PGconn *,Utente*);

#include "auth.c"

#endif