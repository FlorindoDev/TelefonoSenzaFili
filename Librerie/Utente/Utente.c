#include <string.h>


void initUtente(Utente * utente,char nome[MAX_UTENTE], char password[MAX_UTENTE] ,char lingua[MAX_UTENTE], char funzione[MAX_UTENTE]){

    strcpy(utente->nome,nome);
    strcpy(utente->password,password);
    strcpy(utente->lingua,lingua);
    strcpy(utente->funzione,funzione);
    utente->next=NULL;
    utente->prev=NULL;
}

int getUserSocket(Utente * utente){
    return utente->socket;
}

void printUtente(Utente * utente){
    if(utente == NULL) return;
    printf("\n-----\n");
    printf("Nome:%s\nPassowrd:%s\nLingua:%s\nFunzione:%s\nSocket:%d\nProssimo:%p\nPrev:%p\n", utente->nome,utente->password,utente->lingua,utente->funzione,utente->socket,utente->next,utente->prev);
    printf("\n-----\n");
}

Utente * getNext(Utente * utente){
    if(utente != NULL)return utente->next;
}