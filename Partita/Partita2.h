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

//ROBA SPERIMENTALE
#include <errno.h>
#include <sys/socket.h>
#include <poll.h>

#define TIMEOUT 1000

//FINE

#include "../Librerie/Stanze/Stanze.h"
#include "../Librerie/ThreadConnessione/ThreadConnessione.h"
#include "../Librerie/MessageEditor/MessageEditor.h"
#include "../Librerie/GestioneConnessione/GestioneConnessione.h"

#define GAME_PHRASE_MAX_SIZE 5000
#define MIN_PLAYER 2

pthread_cond_t cond_stato = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_stato = PTHREAD_MUTEX_INITIALIZER;

pthread_cond_t cond_chat = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_chat = PTHREAD_MUTEX_INITIALIZER;


void ChooseAction(int *, char *, Utente *);

void broadcast(int * , char * );

void addPlayer(Utente *);

void addNameToMessage(char *, Utente *);

void timerHomeMade(int, int);

void * thread_Game(void *);

void Game();

void chatPause();

void rimuoviGiocatore(Utente * utente);

int riceviRispostaConTimeout(int socket, char *buffer, size_t size, int timeout_seconds);

void propagateGamePhrase();

int isSocketConnected();

void addAllPlayersWaiting();
void riprendiChat();

int aggiungiProssimoDallaCoda();

void printList();

char* Traduzione(char* s, Utente* , Utente* );

size_t write_callback(void *ptr, size_t size, size_t nmemb, char *data);

void broadcastTraduzione(int * sender_socket, char * sender_messagge,Utente * last_user);

#endif