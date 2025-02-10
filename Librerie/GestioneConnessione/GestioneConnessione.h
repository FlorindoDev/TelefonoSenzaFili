#ifndef CONNESSIONE_MANAGMENT_H
#define CONNESSIONE_MANAGMENT_H

#include <signal.h>
#define EXIT_MESSAGE "c0886be5e66f118ea41bd90727881825"

//Funzioni per gestione socket
int creaSocket(struct sockaddr_in *, int);
int chiudiSocket(int);
char* riceviRisposta(int,char*, int);
void mandaMessaggio(int, char*);
char* riceviRispostaSignal(int, char *, int /* , pthread_key_t */);
char* riceviRispostaGame(int socket, char * buffer, int grandezza_buffer ,pthread_key_t, pthread_t );



#include "GestioneConnessione.c"

#endif