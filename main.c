#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "include/connectionDB.h"
#include "include/auth.h"


int main(){

    PGconn* conn=connect_to_DB();

    Utente user;

    user.nome="Florindo";
    user.password="frociodimerda";
    user.lingua="srilankese";

    //login(conn,&user);

    register_user(conn,&user);
}


