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

pthread_cond_t cond_stato = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex_stato = PTHREAD_MUTEX_INITIALIZER;


void ChooseAction(int *, char *, Utente *);

void broadcast(int * , char * );

void addPlayer(Utente *);

void addNameToMessage(char *, Utente *);

#endif