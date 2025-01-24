#include <stdio.h>
#include <stdlib.h>
#include <libpq-fe.h>

#include "include/connectionDB.h"
#include "include/auth.h"
<<<<<<< HEAD
#include "Prove Socket/server.c"
=======
>>>>>>> c409c2b5b7b82bf9ca566b88c23c05a0d329f410


int main(){

<<<<<<< HEAD
    

    //Utente user;

    //user.nome="Florindo";
    //user.password="frociodimerda";
    //user.lingua="srilankese";

    //login(conn,&user);

    //register_user(conn,&user);
    mainServer();
=======
    PGconn* conn=connect_to_DB();

    Utente user;

    user.nome="Florindo";
    user.password="frociodimerda";
    user.lingua="srilankese";

    //login(conn,&user);

    register_user(conn,&user);
>>>>>>> c409c2b5b7b82bf9ca566b88c23c05a0d329f410
}


