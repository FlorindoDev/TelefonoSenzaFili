#ifndef STANZE_H
#define STANZE_H

#include <pthread.h>
#include "../Utente.h"

#define MAX_LEN 50


enum Stato {
    CREATA,
    INIZIATA,
    SOSPESA,
    FINITA,
    CHIUSA
};



typedef struct Stanza{
    char nomeStanza[MAX_LEN];
    enum Stato stato;
    Utente proprietario;
    Utente * listaPartecipanti;
    struct Stanza * next;
    
}Stanza;

/*Testa della lista
- `next`: punta al primo nodo
- `light`: semaforo 
*/
typedef struct ListStanze{

    struct Stanza * next;
    pthread_mutex_t light;
    
}ListStanze;


/*inserisce la stanza nella lista
Prende:
- `liststanze`: nodo iniziale(origine)
- `new`: il nodo da inserire

*/
void inserisciStanza(ListStanze* liststanze, Stanza* new);


/*Controla se esiste la stanza nella lista
Prende:
- `liststanze`: nodo iniziale(origine)
- `check`: controlla se questo nodo è nella lista
`return` ritorna `0` se non esiste e `1` se esiste
*/
int existStanza(ListStanze* liststanze, Stanza* check);

/*
Inizializza la testa della lista
`return` la testa con next a null è il semaforo inizializato
*/
ListStanze* initTesta();

/*
- `liststanze`: nodo iniziale(origine)
- `nomeStanza`: nome dell stanza che voglio cancellare
`return` ritorna `1` se cancelata e `0` se non cancellata
*/
int removeStanza(ListStanze* liststanze, char* nomeStanza);

#include "Stanze.c"

#endif