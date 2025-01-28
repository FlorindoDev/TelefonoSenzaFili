#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "Librerie/connessioneDB/connectionDB.h"
#include "Librerie/Autenticazione/auth.h"
#include "Client-Server/server.c"

int main(){

    

    //Utente user;

    //user.nome="Florindo";
    //user.password="frociodimerda";
    //user.lingua="srilankese";

    //login(conn,&user);

    //register_user(conn,&user);
    mainServer();
}


