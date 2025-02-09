#ifndef STANZE_H
#define STANZE_H

#include <pthread.h>
#include "../Utente/Utente.h"

#define MAX_LEN 50
#define PATH_EXE_PARTITA "./PartitaEXE"


enum Stato {
    CREATA,
    INIZIATA,
    SOSPESA,
    FINITA,
    CHIUSA
};

enum Direzione {
    ASC,
    DESC
};



typedef struct Stanza{
    char nomeStanza[MAX_LEN];
    enum Stato stato;
    Utente proprietario;
    Utente * listaPartecipanti;
    pid_t pid_proccesso_stanza;
    unsigned short int port;
    unsigned short int num_players;
    enum Direzione direzione;
    pthread_mutex_t light;
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
int existStanza(ListStanze* liststanze, char * nomeStanza);

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


/*cancella e libera la memoria del intera lista
- `liststanze`: nodo iniziale(origine)
`return` NULL
*/
ListStanze * freeStanze(ListStanze* liststanze);


/* riposra in modo formattato tutte le stanze
- `liststanze`: nodo iniziale(origine)
`return` la stringa formattata
*/
char*  showStanze(ListStanze* liststanze);

/*
stampa la lista di stanze
- `liststanze*`: nodo iniziale(origine) 
*/
void printStanze(ListStanze* liststanze);

/*
inizializza stanza
- `stanza*`: puntatore alla stanza da inizializzare
- `utente*`: puntatore al proprietario
- `nomeStanza*`: stringa contenente nome della stanza
- `dir`: enum per identificare la direzione del gioco
- `fd*`: file descriptor
*/
void initStanza(Stanza * stanza, Utente* utente, char * nomeStanza, enum Direzione dir, int * fd);


pid_t creazioneProcessoStanza(int* fd);

int returnPortaPartita(int* fd);

int existUtente(Stanza* , char * );

/*
da il prossimo del utente in base alla direzione
- `Stanza*`: puntatore alla lista 
- `Utente*`: utente da voler vedere il prossimo
`return` il prossimo
*/
Utente * getNextInOrder(Stanza* , Utente * );

/*
setta il prossimo del utente in base alla direzione
- `Stanza*`: puntatore alla lista 
- `Utente*`: utente da voler inserire 
`return` successo
*/
int setNextInOrder(Stanza* , Utente * );

enum Stato getStato(Stanza*, pthread_mutex_t*);

void setIniziata(Stanza* stanza_corrente, pthread_mutex_t* mutex_stato)



#include "Stanze.c"

#endif