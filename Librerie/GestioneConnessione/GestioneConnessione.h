#ifndef CONNESSIONE_MANAGMENT_H
#define CONNESSIONE_MANAGMENT_H

#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <poll.h>
#define EXIT_MESSAGE "c0886be5e66f118ea41bd90727881825"

//Funzioni per gestione socket
int creaSocket(struct sockaddr_in *, int);
int chiudiSocket(int);
char* riceviRisposta(int,char*, int);
void mandaMessaggio(int, char*);
char* riceviRispostaSignal(int, char *, int /* , pthread_key_t */);
char* riceviRispostaGame(int socket, char * buffer, int grandezza_buffer ,pthread_key_t, pthread_t );
void sendLingua(int *, char * , char * );
int riceviRispostaConTimeout(int , char *, size_t , int );

int isSocketConnected(int);


#include "GestioneConnessione.c"

#endif