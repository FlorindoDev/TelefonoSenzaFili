#include <string.h>


void initUtente(Utente * utente,char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE], int socket){

    strcpy(utente->nome,nome);
    strcpy(utente->password,password);
    strcpy(utente->lingua,lingua);
    strcpy(utente->funzione,funzione);
    utente->socket=socket;
    utente->next=NULL;
    utente->prev=NULL;
}

int getUserSocket(Utente * utente){
    if(utente!= NULL)return utente->socket;
    return -1;
}

void printUtente(Utente * utente){
    if(utente == NULL) return;
    printf("\n-----\n");
    printf("Nome:%s\nPassowrd:%s\nLingua:%s\nFunzione:%s\nSocket:%d\nThread: %ld\nProssimo:%p\nPrev:%p\n", utente->nome,utente->password,utente->lingua,utente->funzione,utente->socket,utente->thread,utente->next,utente->prev);
    printf("\n-----\n");
}

Utente * getNext(Utente * utente){
    if(utente != NULL)return utente->next;
}


void setPthread(Utente * utente , pthread_t tid){
    utente->thread=tid;
}


int stampaLista(Utente* utente){

    Utente * in_esame = utente;
    while(in_esame != NULL){

        if(in_esame!=NULL){
            printUtente(in_esame);
        }
        in_esame = getNext(in_esame);
    }

}

