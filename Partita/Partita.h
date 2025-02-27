#ifndef PARTITA_H
#define PARTITA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <curl/curl.h>
#include <jansson.h>

#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/ThreadConnessione/ThreadConnessione.h"
#include "../Librerie/MessageEditor/MessageEditor.h"
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"

#define MAX_PAROLA 5000
#define MIN_PLAYER 2


/* mutex e cond per mettere in pasua la chat il gioco e client in coda */
pthread_cond_t cond_stato = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_stato = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_chat = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_chat = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_game = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_game = PTHREAD_MUTEX_INITIALIZER;

pthread_key_t flag_key_game;


//conteggio di persone che vogliono entrare
int inCoda=0;

//varabbili per la connessione
int server_fd;
struct sockaddr_in server_addr;
socklen_t addr_len = sizeof(server_addr);
Stanza stanza_corrente;


int AperturaSocket();
void gestioneNuovaConnessione(int *, char *, Utente *, Message);
char* Traduzione(char*, Utente*, Utente*);
void broadcastTraduzione(int *, char *, Utente *);


void Game();
void ThreadKill();
void timerHomeMade(int , int); //tempo , quante volte 


void * Thread_Game(void *args);


void CloseThread(int);
void NoCrashHendler(int);
void HelndlerSetter(struct sigaction *, __sighandler_t,int,int);



#endif