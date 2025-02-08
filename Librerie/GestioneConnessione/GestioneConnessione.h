#ifndef CONNESSIONE_MANAGMENT_H
#define CONNESSIONE_MANAGMENT_H

//Funzioni per gestione socket
int creaSocket(struct sockaddr_in *, int);
int chiudiSocket(int);
char* riceviRisposta(int,char*, int);
void mandaMessaggio(int, char*);

#include "GestioneConnessione.c"

#endif