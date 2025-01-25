#ifndef STANZE_H
#define STANZE_H

#include "../Utente.h"

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
    Utente proprietario;
    Utente * listaPartecipanti;
    struct Stanza * next;
    
}Stanza;

#endif